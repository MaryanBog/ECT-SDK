#include <cstdlib>
#include <iostream>
#include <cmath>

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

int main()
{
    // Wide bounds to avoid saturation masking sign behavior.
    LinearFOperator    f;
    LinearEOperator    e(0.8);
    LinearFInvOperator finv;
    LinearGOperator    g(1.0, -1e9, 1e9);

    Controller c(f, e, finv, g);

    const double positives[] = { 1e-12, 1e-6, 1e-3, 0.1, 1.0, 10.0, 1e6 };
    const double negatives[] = { -1e-12, -1e-6, -1e-3, -0.1, -1.0, -10.0, -1e6 };

    // Delta = 0 should map to u = 0 for linear operators
    const double u0 = c.update(0.0);
    require_true(u0 == 0.0, "Sign preservation violated: u(0) expected to be 0 for linear operators");

    // Positive deltas -> non-negative outputs
    for (double d : positives)
    {
        const double u = c.update(d);
        require_true(u >= 0.0, "Sign preservation violated: delta > 0 produced u < 0");
    }

    // Negative deltas -> non-positive outputs
    for (double d : negatives)
    {
        const double u = c.update(d);
        require_true(u <= 0.0, "Sign preservation violated: delta < 0 produced u > 0");
    }

    std::cout << "[PASS] sign_preservation_test: sign is preserved for negative/zero/positive deltas." << std::endl;
    return 0;
}
