#pragma once
#include "solver.h"
#include <vector>

namespace PhysicaEngine {

struct KinematicsParams {
    double x0;        // initial x (m)
    double y0;        // initial y (m)
    double vx0;       // initial vx (m/s)
    double vy0;       // initial vy (m/s)
    double mass;      // kg
    double drag_coef; // Cd (0 = no drag)
    double area;      // cross-section m²
    double rho_air;   // air density kg/m³ (default 1.225)
    double g;         // gravity m/s² (default 9.81)
    double dt;        // time step (s)
    double t_end;     // simulation end (s)
};

struct ProjectilePoint {
    double t, x, y, vx, vy, speed, ke;
};

// Run projectile simulation, returns time series
std::vector<ProjectilePoint> simulate_projectile(const KinematicsParams& p);

// Circular motion
struct CircularPoint { double t, x, y, vx, vy, ax, ay; };
std::vector<CircularPoint> simulate_circular(double r, double omega, double t_end, double dt);

// Simple harmonic oscillator (spring-mass)
struct SHMPoint { double t, x, v, a, ke, pe; };
std::vector<SHMPoint> simulate_shm(double k, double m, double x0, double v0,
                                    double t_end, double dt);

} // namespace PhysicaEngine