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
    // Use wide bounds so saturation does NOT interfere with monotonicity checks.
    LinearFOperator    f;
    LinearEOperator    e(0.8);
    LinearFInvOperator finv;
    LinearGOperator    g(1.0, -1e9, 1e9);

    Controller c(f, e, finv, g);

    // Strictly increasing input grid
    const double inputs[] = { -10.0, -2.0, -1.0, -0.5, -0.1, 0.0, 0.1, 0.5, 1.0, 2.0, 10.0 };
    const int N = static_cast<int>(sizeof(inputs) / sizeof(inputs[0]));

    // Compute outputs
    double prev_u = c.update(inputs[0]);

    for (int i = 1; i < N; ++i)
    {
        const double delta = inputs[i];
        const double u = c.update(delta);

        // For monotonic non-decreasing mapping:
        // delta_i > delta_{i-1}  =>  u_i >= u_{i-1}
        require_true(u >= prev_u, "Monotonicity violated: u decreased while delta increased");

        prev_u = u;
    }

    std::cout << "[PASS] monotonicity_test: u is non-decreasing over an increasing delta grid." << std::endl;
    return 0;
}
