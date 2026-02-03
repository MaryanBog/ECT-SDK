#include <iostream>
#include <iomanip>
#include <cmath>

#include "ect_sdk.hpp"

using namespace ect::sdk;

int main()
{
    // --- ECT operators ---
    LinearFOperator    f;
    LinearEOperator    e(0.8);                 // contraction strength
    LinearFInvOperator finv;

    // Tight actuator bounds to force saturation.
    const double UMIN = -1.0;
    const double UMAX =  1.0;
    LinearGOperator    g(1.0, UMIN, UMAX);

    Controller controller(f, e, finv, g);

    // --- Simple 1D plant: position integrates control command ---
    // This is intentionally minimal: pos_{k+1} = pos_k + u_k
    // Target is constant, error is delta = target - pos
    const double target = 100.0;
    double pos = 0.0;

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Saturation stress loop | target=" << target
              << " | bounds=[" << UMIN << ", " << UMAX << "]"
              << " | alpha=0.8" << std::endl;

    for (int k = 0; k < 120; ++k)
    {
        const double delta = target - pos;
        const double u = controller.update(delta);

        // Plant update (external to controller)
        pos += u;

        // Helper flag to make saturation obvious in log
        const char* sat =
            (u <= UMIN) ? "SAT_MIN" :
            (u >= UMAX) ? "SAT_MAX" :
                          "FREE";

        std::cout
            << "Step " << std::setw(3) << k
            << " | delta=" << std::setw(12) << delta
            << " | u="     << std::setw(10) << u
            << " | pos="   << std::setw(12) << pos
            << " | " << sat
            << std::endl;
    }

    return 0;
}
