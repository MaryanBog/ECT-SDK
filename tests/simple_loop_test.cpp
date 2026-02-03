#include <iostream>

#include "ect_sdk.hpp"

using namespace ect::sdk;

int main()
{
    // --- Операторы ---
    LinearFOperator    f;
    LinearEOperator    e(0.8);          // alpha = 0.8
    LinearFInvOperator finv;
    LinearGOperator    g(1.0, -1e9, 1e9);

    // --- Контроллер ---
    Controller controller(f, e, finv, g);

    double delta = 1.0;

    for (int i = 0; i < 10; ++i)
    {
        double u = controller.update(delta);
        delta = delta - u;

        std::cout
            << "Step " << i
            << " | u = " << u
            << " | new delta = " << delta
            << std::endl;
    }

    return 0;
}
