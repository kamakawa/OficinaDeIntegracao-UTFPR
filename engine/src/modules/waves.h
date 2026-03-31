#pragma once
#include <vector>
#include <cmath>

namespace PhysicaEngine {

// ──────────────────────────────────────────────
//  Single wave source
// ──────────────────────────────────────────────
struct WaveSource {
    double amplitude;  // A (m)
    double frequency;  // f (Hz)
    double phase;      // φ (rad)
    double x0;         // source position x (m), for 2D
    double y0;         // source position y (m), for 2D
    double wave_speed; // v (m/s)
};

// 1D — superposition at position x over time
struct Wave1DPoint { double t, x, y; };
std::vector<Wave1DPoint> simulate_superposition_1d(
    const std::vector<WaveSource>& sources,
    double x, double t_end, double dt
);

// 2D interference grid — snapshot at time t
struct Wave2DPoint { double x, y, amplitude; };
std::vector<Wave2DPoint> simulate_interference_2d(
    const std::vector<WaveSource>& sources,
    double t,
    double x_min, double x_max,
    double y_min, double y_max,
    int nx, int ny
);

// Standing wave — modes in a string of length L
struct StandingWavePoint { double x, y; };
std::vector<StandingWavePoint> simulate_standing_wave(
    double L, int mode, double A,
    double wave_speed, double t,
    int nx = 200
);

// Doppler effect — observed frequency
double doppler_frequency(double f_source, double v_sound,
                          double v_observer, double v_source,
                          bool observer_approaching, bool source_approaching);

// Sound intensity (dB)
double sound_intensity_db(double I, double I0 = 1e-12);

} // namespace PhysicaEngine