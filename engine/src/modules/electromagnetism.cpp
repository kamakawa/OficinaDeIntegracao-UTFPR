#include "electromagnetism.h"
#include "../solver.h"
#include <cmath>
#include <stdexcept>

namespace PhysicaEngine {

// ──────────────────────────────────────────────
//  Electric field + potential
// ──────────────────────────────────────────────
Vec2 electric_field(const std::vector<PointCharge>& charges, double px, double py) {
    double Ex = 0, Ey = 0;
    for (auto& c : charges) {
        double dx = px - c.x, dy = py - c.y;
        double r2 = dx*dx + dy*dy;
        if (r2 < 1e-18) continue;
        double r  = std::sqrt(r2);
        double E  = K_E * c.q / r2;
        Ex += E * dx / r;
        Ey += E * dy / r;
    }
    return { Ex, Ey };
}

double electric_potential(const std::vector<PointCharge>& charges, double px, double py) {
    double V = 0;
    for (auto& c : charges) {
        double r = std::sqrt((px-c.x)*(px-c.x) + (py-c.y)*(py-c.y));
        if (r < 1e-9) continue;
        V += K_E * c.q / r;
    }
    return V;
}

std::vector<FieldPoint> electric_field_map(
    const std::vector<PointCharge>& charges,
    double x_min, double x_max,
    double y_min, double y_max,
    int nx, int ny)
{
    std::vector<FieldPoint> out;
    out.reserve(nx * ny);
    double dx = (x_max - x_min) / (nx - 1);
    double dy = (y_max - y_min) / (ny - 1);
    for (int j = 0; j < ny; ++j) {
        for (int i = 0; i < nx; ++i) {
            double px = x_min + i * dx;
            double py = y_min + j * dy;
            auto   E  = electric_field(charges, px, py);
            double V  = electric_potential(charges, px, py);
            double Emag = std::sqrt(E.x*E.x + E.y*E.y);
            out.push_back({ px, py, E.x, E.y, Emag, V });
        }
    }
    return out;
}

// ──────────────────────────────────────────────
//  Magnetic field — Biot-Savart (infinite wire)
//  B = μ₀I / (2πr)  direction: out-of-plane (Bz)
// ──────────────────────────────────────────────
double magnetic_field_wire(const std::vector<MagneticWire>& wires, double px, double py) {
    double Bz = 0;
    for (auto& w : wires) {
        double dx = px - w.x, dy = py - w.y;
        double r2 = dx*dx + dy*dy;
        if (r2 < 1e-18) continue;
        // Sign: right-hand rule — current in +z gives B circling CCW
        // Bz contribution (cross product: r × ẑ) = magnitude with sign from geometry
        double r  = std::sqrt(r2);
        double B  = (MU_0 * w.I) / (2.0 * M_PI * r2);
        // Out-of-plane component (for 2D, simplified as signed magnitude)
        Bz += B * r; // simplified; for full 3D use Vec3
    }
    return Bz;
}

// ──────────────────────────────────────────────
//  Lorentz trajectory  F = q(E + v×B)
//  State: [x, y, vx, vy]
// ──────────────────────────────────────────────
std::vector<LorentzPoint> simulate_lorentz(
    double q, double m,
    double x0, double y0,
    double vx0, double vy0,
    double Bz, double Ex, double Ey,
    double t_end, double dt)
{
    ODEFunc ode = [&](double, const State& y) -> Derivative {
        double vx = y[2], vy = y[3];
        // F = q(E + v×B) — in 2D: vx B_z and vy Bz
        double ax = (q / m) * (Ex + vy * Bz);
        double ay = (q / m) * (Ey - vx * Bz);
        return { vx, vy, ax, ay };
    };

    auto snaps = integrate_rk4(ode, 0.0, t_end, {x0, y0, vx0, vy0}, dt);
    std::vector<LorentzPoint> out;
    out.reserve(snaps.size());
    for (auto& s : snaps) {
        double vx = s.state[2], vy = s.state[3];
        out.push_back({ s.t, s.state[0], s.state[1], vx, vy,
                        std::sqrt(vx*vx + vy*vy) });
    }
    return out;
}

// ──────────────────────────────────────────────
//  RC circuit  Vc(t) = V0(1 - e^(-t/RC))
// ──────────────────────────────────────────────
std::vector<CircuitPoint> simulate_rc(double V0, double R, double C,
                                       bool charging, double t_end, double dt)
{
    std::vector<CircuitPoint> out;
    double tau = R * C;
    for (double t = 0; t <= t_end; t += dt) {
        double Vc = charging
            ? V0 * (1.0 - std::exp(-t / tau))
            : V0 * std::exp(-t / tau);
        double I  = charging
            ? (V0 / R) * std::exp(-t / tau)
            : -(V0 / R) * std::exp(-t / tau);
        double energy = 0.5 * C * Vc * Vc;
        out.push_back({ t, Vc, 0.0, I, energy });
    }
    return out;
}

// ──────────────────────────────────────────────
//  RLC damped oscillator
//  State: [q (charge), I (current)]
//  L·dI/dt + R·I + q/C = V0 (step)
// ──────────────────────────────────────────────
std::vector<CircuitPoint> simulate_rlc(double V0, double R, double L, double C,
                                        double t_end, double dt)
{
    ODEFunc ode = [&](double, const State& y) -> Derivative {
        double q = y[0], I = y[1];
        double dI = (V0 - R*I - q/C) / L;
        return { I, dI };
    };

    auto snaps = integrate_rk4(ode, 0.0, t_end, {0.0, 0.0}, dt);
    std::vector<CircuitPoint> out;
    out.reserve(snaps.size());
    for (auto& s : snaps) {
        double q = s.state[0], I = s.state[1];
        double Vc = q / C;
        double Vl = L * (snaps.size() > 1 ? 0 : 0); // simplified
        double energy = 0.5 * L * I * I + 0.5 * q * q / C;
        out.push_back({ s.t, Vc, Vl, I, energy });
    }
    return out;
}

} // namespace PhysicaEngine