#ifndef ECT_SDK_F_OPERATOR_HPP
#define ECT_SDK_F_OPERATOR_HPP

namespace ect::sdk
{
    class FOperator
    {
    public:
        virtual ~FOperator() = default;
        virtual double apply(double delta) const = 0;
    };

    class LinearFOperator final : public FOperator
    {
    public:
        double apply(double delta) const override;
    };
}

#endif // ECT_SDK_F_OPERATOR_HPP
