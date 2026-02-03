#include "ect_e_operator.hpp"

namespace ect::sdk
{
    LinearEOperator::LinearEOperator(double gain)
        : k_(gain)
    {
    }

    double LinearEOperator::apply(double x) const
    {
        return k_ * x; // alpha * x
    }
}
