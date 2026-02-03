#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>

#include "ect_sdk.hpp"

using namespace ect::sdk;

static void require_true(bool cond, const char* msg)
{
    if (!cond)
    {
        std::cerr << "[FAIL] " << msg << std::endl;
        std::exit(1);
    }
}

static bool is_finite(double x)
{
    return std::isfinite(x);
}

int main()
{
    // Use small bounds to make saturation guaranteed and obvious.
    const double UMIN = -1.0;
    const double UMAX =  1.0;

    LinearFOperator    f;
    LinearEOperator    e(0.8);
    LinearFInvOperator finv;
    LinearGOperator    g(1.0, UMIN, UMAX);

    Controller c(f, e, finv, g);

    // Extreme and representative inputs (both signs).
    const double inputs[] = {
        0.0,
        1e-12,
        -1e-12,
        1.0,
        -1.0,
        1e3,
        -1e3,
        1e9,
        -1e9,
        1e12,
        -1e12
    };
    const int N = static_cast<int>(sizeof(inputs) / sizeof(inputs[0]));

    for (int i = 0; i < N; ++i)
    {
        const double delta = inputs[i];
        const double u = c.update(delta);

        require_true(is_finite(u), "Boundedness violated: u is not finite (NaN/Inf)");
        require_true(u >= UMIN, "Boundedness violated: u < UMIN");
        require_true(u <= UMAX, "Boundedness violated: u > UMAX");
    }

    // Stronger property for saturation:
    // For sufficiently large positive delta, u should hit UMAX;
    // for sufficiently large negative delta, u should hit UMIN.
    const double u_pos = c.update(1e12);
    const double u_neg = c.update(-1e12);

    require_true(u_pos == UMAX, "Saturation expected: u_pos should equal UMAX");
    require_true(u_neg == UMIN, "Saturation expected: u_neg should equal UMIN");

    std::cout << "[PASS] boundedness_test: output finite and within [" << UMIN << ", " << UMAX << "], saturation confirmed." << std::endl;
    return 0;
}
