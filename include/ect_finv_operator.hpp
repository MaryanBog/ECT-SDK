#ifndef ECT_SDK_FINV_OPERATOR_HPP
#define ECT_SDK_FINV_OPERATOR_HPP

namespace ect::sdk
{
    class FInvOperator
    {
    public:
        virtual ~FInvOperator() = default;
        virtual double apply(double x) const = 0;
    };

    class LinearFInvOperator final : public FInvOperator
    {
    public:
        double apply(double x) const override;
    };
}

#endif // ECT_SDK_FINV_OPERATOR_HPP
