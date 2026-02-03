#include "ect_g_operator.hpp"

namespace ect::sdk
{
    LinearGOperator::LinearGOperator(double gain, double u_min, double u_max)
        : k_(gain), u_min_(u_min), u_max_(u_max)
    {
    }

    double LinearGOperator::apply(double delta) const
    {
        const double u = k_ * delta;

        if (u < u_min_) return u_min_;
        if (u > u_max_) return u_max_;
        return u;
    }
}
