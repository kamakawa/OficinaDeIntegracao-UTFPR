#pragma once
#include <functional>
#include <vector>

namespace PhysicaEngine {

// Generic state vector
using State = std::vector<double>;
using Derivative = std::vector<double>;
using ODEFunc = std::function<Derivative(double t, const State&)>;

// RK4 — 4th order Runge-Kutta (general purpose)
State rk4_step(const ODEFunc& f, double t, const State& y, double dt);

// Velocity Verlet — optimal for conservative forces (N-body, spring)
struct VerletState {
    std::vector<double> pos;
    std::vector<double> vel;
};
using ForceFunc = std::function<std::vector<double>(double t, const VerletState&)>;
VerletState verlet_step(const ForceFunc& force, double t, const VerletState& s, double dt);

// Integrate over time range, returns vector of (t, state) snapshots
struct Snapshot {
    double t;
    State  state;
};
std::vector<Snapshot> integrate_rk4(
    const ODEFunc& f,
    double t0, double t_end,
    const State& y0,
    double dt,
    int    max_steps = 100000
);

} // namespace PhysicaEngine