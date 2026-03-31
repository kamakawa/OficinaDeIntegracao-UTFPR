#include "solver.h"
#include <stdexcept>

namespace PhysicaEngine {

// ──────────────────────────────────────────────
//  Helpers
// ──────────────────────────────────────────────
static Derivative add(const Derivative& a, const Derivative& b) {
    Derivative r(a.size());
    for (size_t i = 0; i < a.size(); ++i) r[i] = a[i] + b[i];
    return r;
}
static Derivative scale(const Derivative& a, double s) {
    Derivative r(a.size());
    for (size_t i = 0; i < a.size(); ++i) r[i] = a[i] * s;
    return r;
}
static State apply(const State& y, const Derivative& d, double dt) {
    State r(y.size());
    for (size_t i = 0; i < y.size(); ++i) r[i] = y[i] + d[i] * dt;
    return r;
}

// ──────────────────────────────────────────────
//  RK4 single step
// ──────────────────────────────────────────────
State rk4_step(const ODEFunc& f, double t, const State& y, double dt) {
    auto k1 = f(t,            y);
    auto k2 = f(t + dt * 0.5, apply(y, k1, dt * 0.5));
    auto k3 = f(t + dt * 0.5, apply(y, k2, dt * 0.5));
    auto k4 = f(t + dt,       apply(y, k3, dt));

    State result(y.size());
    for (size_t i = 0; i < y.size(); ++i)
        result[i] = y[i] + (dt / 6.0) * (k1[i] + 2*k2[i] + 2*k3[i] + k4[i]);
    return result;
}

// ──────────────────────────────────────────────
//  Velocity Verlet
// ──────────────────────────────────────────────
VerletState verlet_step(const ForceFunc& force, double t, const VerletState& s, double dt) {
    auto a0 = force(t, s);
    VerletState mid;
    mid.pos.resize(s.pos.size());
    mid.vel = s.vel;
    for (size_t i = 0; i < s.pos.size(); ++i)
        mid.pos[i] = s.pos[i] + s.vel[i]*dt + 0.5*a0[i]*dt*dt;

    VerletState next;
    next.pos = mid.pos;
    auto a1 = force(t + dt, mid);
    next.vel.resize(s.vel.size());
    for (size_t i = 0; i < s.vel.size(); ++i)
        next.vel[i] = s.vel[i] + 0.5*(a0[i] + a1[i])*dt;
    return next;
}

// ──────────────────────────────────────────────
//  Full integration (RK4)
// ──────────────────────────────────────────────
std::vector<Snapshot> integrate_rk4(
    const ODEFunc& f,
    double t0, double t_end,
    const State& y0,
    double dt,
    int max_steps)
{
    if (dt <= 0) throw std::invalid_argument("dt must be positive");
    std::vector<Snapshot> out;
    out.reserve(static_cast<size_t>((t_end - t0) / dt) + 2);

    State y = y0;
    double t = t0;
    int steps = 0;

    out.push_back({t, y});
    while (t < t_end && steps < max_steps) {
        double h = std::min(dt, t_end - t);
        y = rk4_step(f, t, y, h);
        t += h;
        out.push_back({t, y});
        ++steps;
    }
    return out;
}

} // namespace PhysicaEngine