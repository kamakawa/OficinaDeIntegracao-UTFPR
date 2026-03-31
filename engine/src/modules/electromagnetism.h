#pragma once
#include <vector>
#include <cmath>

namespace PhysicaEngine {

constexpr double K_E   = 8.9875517923e9; // Coulomb constant N·m²/C²
constexpr double EPS_0 = 8.854187817e-12; // permittivity of free space
constexpr double MU_0  = 1.2566370614e-6; // permeability of free space

// ──────────────────────────────────────────────
//  Point charge
// ──────────────────────────────────────────────
struct PointCharge {
    double q;   // charge (C)
    double x;   // position x (m)
    double y;   // position y (m)
};

// Electric field at (px, py) from a set of charges
struct Vec2 { double x, y; };
Vec2 electric_field(const std::vector<PointCharge>& charges, double px, double py);

// Electric potential V at point
double electric_potential(const std::vector<PointCharge>& charges, double px, double py);

// Field map on a grid — returns {x, y, Ex, Ey, |E|, V}
struct FieldPoint {
    double x, y, Ex, Ey, E_mag, V;
};
std::vector<FieldPoint> electric_field_map(
    const std::vector<PointCharge>& charges,
    double x_min, double x_max,
    double y_min, double y_max,
    int nx, int ny
);

// ──────────────────────────────────────────────
//  Magnetic field — infinite straight wire
// ──────────────────────────────────────────────
struct MagneticWire {
    double I;   // current (A)
    double x;   // wire position x
    double y;   // wire position y
};
// Returns Bz component (out-of-plane for 2D)
double magnetic_field_wire(const std::vector<MagneticWire>& wires, double px, double py);

// ──────────────────────────────────────────────
//  Lorentz force on moving charge
// ──────────────────────────────────────────────
struct LorentzState {
    double x, y;    // position
    double vx, vy;  // velocity
};
// Trajectory of charge in uniform B field (Bz)
struct LorentzPoint { double t, x, y, vx, vy, speed; };
std::vector<LorentzPoint> simulate_lorentz(
    double q, double m,
    double x0, double y0,
    double vx0, double vy0,
    double Bz, double Ex, double Ey,
    double t_end, double dt
);

// ──────────────────────────────────────────────
//  RC / RL / RLC circuit transient
// ──────────────────────────────────────────────
struct CircuitPoint { double t, Vc, Vl, I, energy; };

// RC charge/discharge
std::vector<CircuitPoint> simulate_rc(double V0, double R, double C,
                                       bool charging, double t_end, double dt);
// RLC damped oscillation
std::vector<CircuitPoint> simulate_rlc(double V0, double R, double L, double C,
                                        double t_end, double dt);

} // namespace PhysicaEngine