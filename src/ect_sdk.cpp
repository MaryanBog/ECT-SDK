#include "ect_sdk.hpp"

namespace ect::sdk
{
    Controller::Controller(
        const FOperator&    f,
        const EOperator&    e,
        const FInvOperator& finv,
        const GOperator&    g
    )
    : f_(f)
    , e_(e)
    , finv_(finv)
    , g_(g)
    {
    }

    double Controller::update(double delta) const
    {
        const double x_f    = f_.apply(delta);
        const double x_e    = e_.apply(x_f);
        const double x_finv = finv_.apply(x_e);
        return g_.apply(x_finv);
    }

} // namespace ect::sdk
