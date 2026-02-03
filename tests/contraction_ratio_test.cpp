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

static bool approx_equal(double a, double b, double eps = 1e-12)
{
    return std::fabs(a - b) <= eps;
}

int main()
{
    const double alpha = 0.8;
    const double expected_ratio = 1.0 - alpha; // 0.2

    // Wide bounds so no saturation is involved
    LinearFOperator    f;
    LinearEOperator    e(alpha);
    LinearFInvOperator finv;
    LinearGOperator    g(1.0, -1e9, 1e9);

    Controller c(f, e, finv, g);

    double delta = 1.0;

    // Run several steps and check the ratio delta_{k+1}/delta_k
    for (int k = 0; k < 20; ++k)
    {
        const double u = c.update(delta);
        const double next_delta = delta - u;

        // For delta != 0, ratio should be ~ (1 - alpha)
        if (delta != 0.0)
        {
            const double ratio = next_delta / delta;
            require_true(approx_equal(ratio, expected_ratio, 1e-12),
                         "Contraction ratio violated: delta_{k+1}/delta_k != (1-alpha)");
        }

        delta = next_delta;
    }

    std::cout << "[PASS] contraction_ratio_test: delta_{k+1} = (1-alpha)*delta_k verified (alpha=" 
              << alpha << ")." << std::endl;
    return 0;
}
