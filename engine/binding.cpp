#include <napi.h>
#include <sstream>
#include "src/modules/kinematics.h"
#include "src/modules/waves.h"
#include "src/modules/electromagnetism.h"

using namespace PhysicaEngine;

// ═══════════════════════════════════════════════════════════
//  AsyncWorker base — runs C++ on a libuv thread, resolves
//  a JS Promise on the main thread when done.
// ═══════════════════════════════════════════════════════════
class PhysicsWorker : public Napi::AsyncWorker {
public:
    PhysicsWorker(Napi::Env env)
        : Napi::AsyncWorker(env),
          deferred_(Napi::Promise::Deferred::New(env)) {}

    Napi::Promise GetPromise() { return deferred_.Promise(); }

protected:
    void OnOK() override {
        deferred_.Resolve(BuildResult(Env()));
    }
    void OnError(const Napi::Error& e) override {
        deferred_.Reject(e.Value());
    }

    virtual Napi::Value BuildResult(Napi::Env env) = 0;
    Napi::Promise::Deferred deferred_;
};

// ─────────────────────────────────────────
//  Helper: get double from object with default
// ─────────────────────────────────────────
static double GetDouble(const Napi::Object& obj, const char* key, double def = 0.0) {
    return obj.Has(key) ? obj.Get(key).As<Napi::Number>().DoubleValue() : def;
}
static int GetInt(const Napi::Object& obj, const char* key, int def = 0) {
    return obj.Has(key) ? obj.Get(key).As<Napi::Number>().Int32Value() : def;
}
static bool GetBool(const Napi::Object& obj, const char* key, bool def = false) {
    return obj.Has(key) ? obj.Get(key).As<Napi::Boolean>().Value() : def;
}

// ═══════════════════════════════════════════════════════════
//  PROJECTILE
// ═══════════════════════════════════════════════════════════
class ProjectileWorker : public PhysicsWorker {
public:
    ProjectileWorker(Napi::Env env, KinematicsParams p)
        : PhysicsWorker(env), params_(p) {}
    void Execute() override {
        result_ = simulate_projectile(params_);
    }
    Napi::Value BuildResult(Napi::Env env) override {
        auto arr = Napi::Array::New(env, result_.size());
        for (size_t i = 0; i < result_.size(); ++i) {
            auto& pt = result_[i];
            auto obj = Napi::Object::New(env);
            obj.Set("t",     pt.t);
            obj.Set("x",     pt.x);
            obj.Set("y",     pt.y);
            obj.Set("vx",    pt.vx);
            obj.Set("vy",    pt.vy);
            obj.Set("speed", pt.speed);
            obj.Set("ke",    pt.ke);
            arr[i] = obj;
        }
        return arr;
    }
private:
    KinematicsParams params_;
    std::vector<ProjectilePoint> result_;
};

Napi::Value SimulateProjectile(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsObject())
        throw Napi::TypeError::New(env, "Expected params object");

    auto p_js = info[0].As<Napi::Object>();
    KinematicsParams p;
    p.x0        = GetDouble(p_js, "x0",       0.0);
    p.y0        = GetDouble(p_js, "y0",       0.0);
    p.vx0       = GetDouble(p_js, "vx0",      50.0);
    p.vy0       = GetDouble(p_js, "vy0",      50.0);
    p.mass      = GetDouble(p_js, "mass",     1.0);
    p.drag_coef = GetDouble(p_js, "dragCoef", 0.47);
    p.area      = GetDouble(p_js, "area",     0.01);
    p.rho_air   = GetDouble(p_js, "rhoAir",   1.225);
    p.g         = GetDouble(p_js, "g",        9.81);
    p.dt        = GetDouble(p_js, "dt",       0.01);
    p.t_end     = GetDouble(p_js, "tEnd",     20.0);

    auto* worker = new ProjectileWorker(env, p);
    auto promise = worker->GetPromise();
    worker->Queue();
    return promise;
}

// ═══════════════════════════════════════════════════════════
//  WAVES — 2D interference
// ═══════════════════════════════════════════════════════════
class WaveWorker : public PhysicsWorker {
public:
    WaveWorker(Napi::Env env, std::vector<WaveSource> s,
               double t, double xmin, double xmax,
               double ymin, double ymax, int nx, int ny)
        : PhysicsWorker(env), sources_(s), t_(t),
          xmin_(xmin), xmax_(xmax), ymin_(ymin), ymax_(ymax),
          nx_(nx), ny_(ny) {}

    void Execute() override {
        result_ = simulate_interference_2d(sources_, t_,
            xmin_, xmax_, ymin_, ymax_, nx_, ny_);
    }
    Napi::Value BuildResult(Napi::Env env) override {
        auto arr = Napi::Array::New(env, result_.size());
        for (size_t i = 0; i < result_.size(); ++i) {
            auto obj = Napi::Object::New(env);
            obj.Set("x",   result_[i].x);
            obj.Set("y",   result_[i].y);
            obj.Set("amp", result_[i].amplitude);
            arr[i] = obj;
        }
        return arr;
    }
private:
    std::vector<WaveSource> sources_;
    double t_, xmin_, xmax_, ymin_, ymax_;
    int nx_, ny_;
    std::vector<Wave2DPoint> result_;
};

Napi::Value SimulateWaves(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsObject())
        throw Napi::TypeError::New(env, "Expected params object");

    auto p = info[0].As<Napi::Object>();
    std::vector<WaveSource> sources;

    if (p.Has("sources") && p.Get("sources").IsArray()) {
        auto arr = p.Get("sources").As<Napi::Array>();
        for (uint32_t i = 0; i < arr.Length(); ++i) {
            auto s = arr.Get(i).As<Napi::Object>();
            sources.push_back({
                GetDouble(s, "amplitude",  1.0),
                GetDouble(s, "frequency",  1.0),
                GetDouble(s, "phase",      0.0),
                GetDouble(s, "x0",         0.0),
                GetDouble(s, "y0",         0.0),
                GetDouble(s, "waveSpeed",  340.0)
            });
        }
    }

    auto* worker = new WaveWorker(env, sources,
        GetDouble(p, "t",    0.0),
        GetDouble(p, "xMin", -10.0), GetDouble(p, "xMax", 10.0),
        GetDouble(p, "yMin", -10.0), GetDouble(p, "yMax", 10.0),
        GetInt(p, "nx", 80),         GetInt(p, "ny", 80));

    auto promise = worker->GetPromise();
    worker->Queue();
    return promise;
}

// ═══════════════════════════════════════════════════════════
//  ELECTRIC FIELD MAP
// ═══════════════════════════════════════════════════════════
class FieldWorker : public PhysicsWorker {
public:
    FieldWorker(Napi::Env env, std::vector<PointCharge> charges,
                double xmin, double xmax, double ymin, double ymax, int nx, int ny)
        : PhysicsWorker(env), charges_(charges),
          xmin_(xmin), xmax_(xmax), ymin_(ymin), ymax_(ymax), nx_(nx), ny_(ny) {}

    void Execute() override {
        result_ = electric_field_map(charges_, xmin_, xmax_, ymin_, ymax_, nx_, ny_);
    }
    Napi::Value BuildResult(Napi::Env env) override {
        auto arr = Napi::Array::New(env, result_.size());
        for (size_t i = 0; i < result_.size(); ++i) {
            auto& fp = result_[i];
            auto obj = Napi::Object::New(env);
            obj.Set("x",    fp.x);
            obj.Set("y",    fp.y);
            obj.Set("Ex",   fp.Ex);
            obj.Set("Ey",   fp.Ey);
            obj.Set("Emag", fp.E_mag);
            obj.Set("V",    fp.V);
            arr[i] = obj;
        }
        return arr;
    }
private:
    std::vector<PointCharge> charges_;
    double xmin_, xmax_, ymin_, ymax_;
    int nx_, ny_;
    std::vector<FieldPoint> result_;
};

Napi::Value SimulateElectricField(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsObject())
        throw Napi::TypeError::New(env, "Expected params object");

    auto p = info[0].As<Napi::Object>();
    std::vector<PointCharge> charges;

    if (p.Has("charges") && p.Get("charges").IsArray()) {
        auto arr = p.Get("charges").As<Napi::Array>();
        for (uint32_t i = 0; i < arr.Length(); ++i) {
            auto c = arr.Get(i).As<Napi::Object>();
            charges.push_back({
                GetDouble(c, "q", 1e-9),
                GetDouble(c, "x", 0.0),
                GetDouble(c, "y", 0.0)
            });
        }
    }

    auto* worker = new FieldWorker(env, charges,
        GetDouble(p, "xMin", -5.0), GetDouble(p, "xMax", 5.0),
        GetDouble(p, "yMin", -5.0), GetDouble(p, "yMax", 5.0),
        GetInt(p, "nx", 40),        GetInt(p, "ny", 40));

    auto promise = worker->GetPromise();
    worker->Queue();
    return promise;
}

// ═══════════════════════════════════════════════════════════
//  RLC CIRCUIT
// ═══════════════════════════════════════════════════════════
Napi::Value SimulateRLC(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto p = info[0].As<Napi::Object>();

    double V0 = GetDouble(p, "V0",   10.0);
    double R  = GetDouble(p, "R",    10.0);
    double L  = GetDouble(p, "L",    0.1);
    double C  = GetDouble(p, "C",    1e-4);
    double dt = GetDouble(p, "dt",   1e-5);
    double te = GetDouble(p, "tEnd", 0.05);

    // Run synchronously (fast enough for small dt ranges)
    auto result = simulate_rlc(V0, R, L, C, te, dt);
    auto arr = Napi::Array::New(env, result.size());
    for (size_t i = 0; i < result.size(); ++i) {
        auto obj = Napi::Object::New(env);
        obj.Set("t",      result[i].t);
        obj.Set("Vc",     result[i].Vc);
        obj.Set("I",      result[i].I);
        obj.Set("energy", result[i].energy);
        arr[i] = obj;
    }
    return arr;
}

// ═══════════════════════════════════════════════════════════
//  MODULE INIT
// ═══════════════════════════════════════════════════════════
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("simulateProjectile",     Napi::Function::New(env, SimulateProjectile));
    exports.Set("simulateWaves",          Napi::Function::New(env, SimulateWaves));
    exports.Set("simulateElectricField",  Napi::Function::New(env, SimulateElectricField));
    exports.Set("simulateRLC",            Napi::Function::New(env, SimulateRLC));
    return exports;
}

NODE_API_MODULE(physics_engine, Init)