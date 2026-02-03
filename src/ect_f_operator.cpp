#include "ect_f_operator.hpp"

namespace ect::sdk
{
    double LinearFOperator::apply(double delta) const
    {
        return delta; // kF = 1.0
    }
}
