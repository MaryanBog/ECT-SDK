#include <iostream>

#include "ect_sdk.hpp"

using namespace ect::sdk;

// Простейшая модель серво
double simulate_servo(double current_pos, double command)
{
    double response_gain = 0.3;
    return current_pos + command * response_gain;
}

int main()
{
    // --- Операторы ---
    LinearFOperator    f;
    LinearEOperator    e(0.8);
    LinearFInvOperator finv;
    LinearGOperator    g(1.0, -100.0, 100.0);

    // --- Контроллер ---
    Controller ctrl(f, e, finv, g);

    double target = 45.0;
    double pos    = 0.0;

    for (int i = 0; i < 20; ++i)
    {
        double delta = target - pos;
        double u     = ctrl.update(delta);

        pos = simulate_servo(pos, u);

        std::cout
            << "Step " << i
            << " | delta=" << delta
            << " | u=" << u
            << " | pos=" << pos
            << std::endl;
    }

    return 0;
}
