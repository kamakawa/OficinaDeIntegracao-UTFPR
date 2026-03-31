#include "waves.h"
#include <cmath>

namespace PhysicaEngine {

// ──────────────────────────────────────────────
//  1D Superposition  y(x,t) = ΣA·sin(kx − ωt + φ)
// ──────────────────────────────────────────────
std::vector<Wave1DPoint> simulate_superposition_1d(
    const std::vector<WaveSource>& sources,
    double x, double t_end, double dt)
{
    std::vector<Wave1DPoint> out;
    for (double t = 0; t <= t_end; t += dt) {
        double y = 0;
        for (auto& s : sources) {
            double omega = 2.0 * M_PI * s.frequency;
            double k     = omega / s.wave_speed;
            y += s.amplitude * std::sin(k * x - omega * t + s.phase);
        }
        out.push_back({ t, x, y });
    }
    return out;
}

// ──────────────────────────────────────────────
//  2D Interference  — circular waves from point sources
// ──────────────────────────────────────────────
std::vector<Wave2DPoint> simulate_interference_2d(
    const std::vector<WaveSource>& sources,
    double t,
    double x_min, double x_max,
    double y_min, double y_max,
    int nx, int ny)
{
    std::vector<Wave2DPoint> out;
    out.reserve(nx * ny);
    double dx = (x_max - x_min) / (nx - 1);
    double dy = (y_max - y_min) / (ny - 1);

    for (int j = 0; j < ny; ++j) {
        for (int i = 0; i < nx; ++i) {
            double px = x_min + i * dx;
            double py = y_min + j * dy;
            double total = 0;
            for (auto& s : sources) {
                double r = std::sqrt((px - s.x0)*(px - s.x0) + (py - s.y0)*(py - s.y0));
                if (r < 1e-9) r = 1e-9;
                double omega = 2.0 * M_PI * s.frequency;
                double k     = omega / s.wave_speed;
                // Amplitude decays with 1/sqrt(r) for 2D
                total += (s.amplitude / std::sqrt(r)) * std::cos(k * r - omega * t + s.phase);
            }
            out.push_back({ px, py, total });
        }
    }
    return out;
}

// ──────────────────────────────────────────────
//  Standing wave  y(x,t) = A·sin(nπx/L)·cos(ωt)
// ──────────────────────────────────────────────
std::vector<StandingWavePoint> simulate_standing_wave(
    double L, int mode, double A,
    double wave_speed, double t, int nx)
{
    double omega = mode * M_PI * wave_speed / L;
    double time_factor = std::cos(omega * t);
    std::vector<StandingWavePoint> out;
    out.reserve(nx);
    for (int i = 0; i < nx; ++i) {
        double x = L * i / (nx - 1);
        double y = A * std::sin(mode * M_PI * x / L) * time_factor;
        out.push_back({ x, y });
    }
    return out;
}

// ──────────────────────────────────────────────
//  Doppler effect
// ──────────────────────────────────────────────
double doppler_frequency(double f_source, double v_sound,
                          double v_observer, double v_source,
                          bool observer_approaching, bool source_approaching)
{
    double v_obs = observer_approaching  ? +v_observer : -v_observer;
    double v_src = source_approaching    ? -v_source   : +v_source;
    return f_source * (v_sound + v_obs) / (v_sound + v_src);
}

// ──────────────────────────────────────────────
//  Sound intensity dB
// ──────────────────────────────────────────────
double sound_intensity_db(double I, double I0) {
    return 10.0 * std::log10(I / I0);
}

} // namespace PhysicaEngine