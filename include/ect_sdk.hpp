#ifndef ECT_SDK_HPP
#define ECT_SDK_HPP

#include "ect_f_operator.hpp"
#include "ect_e_operator.hpp"
#include "ect_finv_operator.hpp"
#include "ect_g_operator.hpp"
#include "ect_config.hpp"

namespace ect::sdk
{
    class Controller
    {
    public:
        Controller(
            const FOperator&    f,
            const EOperator&    e,
            const FInvOperator& finv,
            const GOperator&    g
        );

        double update(double delta) const;

    private:
        const FOperator&    f_;
        const EOperator&    e_;
        const FInvOperator& finv_;
        const GOperator&    g_;
    };

} // namespace ect::sdk

#endif // ECT_SDK_HPP
