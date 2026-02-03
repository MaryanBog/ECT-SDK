#include <iostream>

#include "ect_sdk.hpp"

using namespace ect::sdk;

// Простейшая модель изменения высоты дрона
double simulate_drone(double altitude, double thrust)
{
    double k = 0.4;
    return altitude + thrust * k;
}

int main()
{
    // --- Операторы ---
    LinearFOperator    f;
    LinearEOperator    e(0.8);
    LinearFInvOperator finv;
    LinearGOperator    g(1.0, -10.0, 10.0);

    // --- Контроллер ---
    Controller ctrl(f, e, finv, g);

    double target_alt = 10.0;
    double alt        = 0.0;

    for (int i = 0; i < 25; ++i)
    {
        double delta  = target_alt - alt;
        double thrust = ctrl.update(delta);

        alt = simulate_drone(alt, thrust);

        std::cout
            << "Step " << i
            << " | delta=" << delta
            << " | thrust=" << thrust
            << " | altitude=" << alt
            << std::endl;
    }

    return 0;
}
