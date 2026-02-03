#include <cmath>
#include <cstdlib>
#include <iostream>

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
    // Fixed operator configuration
    LinearFOperator    f;
    LinearEOperator    e(0.8);
    LinearFInvOperator finv;
    LinearGOperator    g(1.0, -1e9, 1e9);

    Controller c1(f, e, finv, g);
    Controller c2(f, e, finv, g);

    // Deterministic input sequence (no randomness)
    const double inputs[] = { 1.0, 0.5, 0.25, 0.1, 0.0, -0.1, -1.0, 2.0, -2.0 };
    const int N = static_cast<int>(sizeof(inputs) / sizeof(inputs[0]));

    for (int i = 0; i < N; ++i)
    {
        const double delta = inputs[i];

        const double u1 = c1.update(delta);
        const double u2 = c2.update(delta);

        // exact equality expected for pure linear operators (no FP chaos here)
        require_true(u1 == u2, "Determinism violated: u1 != u2 for same input and config");
    }

    std::cout << "[PASS] determinism_test: identical inputs produced identical outputs." << std::endl;
    return 0;
}
