#include <iostream>
#include <iomanip>
#include <cmath>

#include "ect_sdk.hpp"

using namespace ect::sdk;

static double deterministic_noise(int k)
{
    // Fully deterministic "noise-like" signal:
    // sum of incommensurate sines => looks noisy but is reproducible.
    const double a = 0.05;
    return a * (0.6 * std::sin(0.37 * k) + 0.3 * std::sin(1.11 * k) + 0.1 * std::sin(2.73 * k));
}

int main()
{
    // --- ECT operators ---
    LinearFOperator    f;
    LinearEOperator    e(0.8);
    LinearFInvOperator finv;

    // Tight-ish bounds so we can see if noise causes saturation (usually it should not)
    const double UMIN = -2.0;
    const double UMAX =  2.0;
    LinearGOperator    g(1.0, UMIN, UMAX);

    Controller controller(f, e, finv, g);

    // --- Simple 1D plant: position integrates control command ---
    const double target = 10.0;
    double pos = 0.0;

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Noise injection loop | target=" << target
              << " | bounds=[" << UMIN << ", " << UMAX << "]"
              << " | alpha=0.8"
              << " | noise=deterministic_sine_mix"
              << std::endl;

    for (int k = 0; k < 80; ++k)
    {
        const double delta_true  = target - pos;
        const double noise       = deterministic_noise(k);
        const double delta_noisy = delta_true + noise;

        const double u = controller.update(delta_noisy);

        // Plant update (external)
        pos += u;

        const char* sat =
            (u <= UMIN) ? "SAT_MIN" :
            (u >= UMAX) ? "SAT_MAX" :
                          "FREE";

        std::cout
            << "Step " << std::setw(3) << k
            << " | delta_true="  << std::setw(12) << delta_true
            << " | noise="       << std::setw(10) << noise
            << " | delta_in="    << std::setw(12) << delta_noisy
            << " | u="           << std::setw(10) << u
            << " | pos="         << std::setw(12) << pos
            << " | " << sat
            << std::endl;
    }

    return 0;
}
