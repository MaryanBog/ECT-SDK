#include "ect_finv_operator.hpp"

namespace ect::sdk
{
    double LinearFInvOperator::apply(double x) const
    {
        return x; // kF = 1.0 → x / kF
    }
}
