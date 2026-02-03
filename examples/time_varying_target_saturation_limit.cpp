#include <iostream>
#include <iomanip>
#include <cmath>

#include "ect_sdk.hpp"

using namespace ect::sdk;

int main()
{
    // --- ECT operators ---
    LinearFOperator    f;
    LinearEOperator    e(0.8);
    LinearFInvOperator finv;

    // Much tighter bounds to force saturation (actuator cannot keep up).
const double UMIN = -0.1;
const double UMAX =  0.1;

    LinearGOperator    g(1.0, UMIN, UMAX);

    Controller controller(f, e, finv, g);

    // --- Simple 1D plant ---
    double pos = 0.0;

    // --- Time-varying target: ramp + sine (same as before) ---
    const double base = 0.0;
    const double ramp_rate = 0.02;
    const double A = 1.0;
    const double w = 0.15;

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Time-varying target (SAT LIMIT) | bounds=[" << UMIN << ", " << UMAX << "]"
              << " | alpha=0.8"
              << " | target = base + ramp_rate*k + A*sin(w*k)"
              << " | base=" << base
              << " ramp_rate=" << ramp_rate
              << " A=" << A
              << " w=" << w
              << std::endl;

    for (int k = 0; k < 200; ++k)
    {
        const double target = base + ramp_rate * k + A * std::sin(w * k);

        const double delta = target - pos;
        const double u = controller.update(delta);

        // Plant update (external)
        pos += u;

        const char* sat =
            (u <= UMIN) ? "SAT_MIN" :
            (u >= UMAX) ? "SAT_MAX" :
                          "FREE";

        std::cout
            << "Step " << std::setw(3) << k
            << " | target=" << std::setw(10) << target
            << " | pos="    << std::setw(10) << pos
            << " | delta="  << std::setw(10) << delta
            << " | u="      << std::setw(10) << u
            << " | " << sat
            << std::endl;
    }

    return 0;
}
