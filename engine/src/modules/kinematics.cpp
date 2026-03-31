#include "kinematics.h"
#include <cmath>

namespace PhysicaEngine {

// ──────────────────────────────────────────────
//  Projectile with drag
//  State: [x, y, vx, vy]
// ──────────────────────────────────────────────
std::vector<ProjectilePoint> simulate_projectile(const KinematicsParams& p) {
    const double rho = p.rho_air > 0 ? p.rho_air : 1.225;
    const double g   = p.g > 0 ? p.g : 9.81;
    const double B2  = 0.5 * p.drag_coef * rho * p.area; // drag constant

    ODEFunc ode = [&](double /*t*/, const State& y) -> Derivative {
        double vx = y[2], vy = y[3];
        double speed = std::sqrt(vx*vx + vy*vy);
        double drag  = (p.mass > 0) ? B2 * speed / p.mass : 0.0;
        return { vx, vy, -drag * vx, -g - drag * vy };
    };

    State y0 = { p.x0, p.y0, p.vx0, p.vy0 };
    auto snapshots = integrate_rk4(ode, 0.0, p.t_end, y0, p.dt);

    std::vector<ProjectilePoint> out;
    out.reserve(snapshots.size());
    for (auto& s : snapshots) {
        if (s.state[1] < -0.001) break; // hit ground
        double vx = s.state[2], vy = s.state[3];
        double sp = std::sqrt(vx*vx + vy*vy);
        double ke = 0.5 * p.mass * sp * sp;
        out.push_back({ s.t, s.state[0], s.state[1], vx, vy, sp, ke });
    }
    return out;
}

// ──────────────────────────────────────────────
//  Circular motion (uniform)
// ──────────────────────────────────────────────
std::vector<CircularPoint> simulate_circular(double r, double omega, double t_end, double dt) {
    std::vector<CircularPoint> out;
    for (double t = 0; t <= t_end; t += dt) {
        double theta = omega * t;
        double x  =  r * std::cos(theta);
        double y  =  r * std::sin(theta);
        double vx = -r * omega * std::sin(theta);
        double vy =  r * omega * std::cos(theta);
        double ax = -r * omega * omega * std::cos(theta);
        double ay = -r * omega * omega * std::sin(theta);
        out.push_back({ t, x, y, vx, vy, ax, ay });
    }
    return out;
}

// ──────────────────────────────────────────────
//  Simple harmonic motion (spring-mass)
//  State: [x, v]
// ──────────────────────────────────────────────
std::vector<SHMPoint> simulate_shm(double k, double m, double x0, double v0,
                                    double t_end, double dt) {
    ODEFunc ode = [&](double, const State& y) -> Derivative {
        return { y[1], -(k / m) * y[0] };
    };

    auto snaps = integrate_rk4(ode, 0.0, t_end, {x0, v0}, dt);
    std::vector<SHMPoint> out;
    out.reserve(snaps.size());
    for (auto& s : snaps) {
        double x = s.state[0], v = s.state[1];
        double ke = 0.5 * m * v * v;
        double pe = 0.5 * k * x * x;
        out.push_back({ s.t, x, v, -(k/m)*x, ke, pe });
    }
    return out;
}

} // namespace PhysicaEngine