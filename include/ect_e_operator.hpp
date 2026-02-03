#ifndef ECT_SDK_E_OPERATOR_HPP
#define ECT_SDK_E_OPERATOR_HPP

namespace ect::sdk
{
    class EOperator
    {
    public:
        virtual ~EOperator() = default;
        virtual double apply(double x) const = 0;
    };

    class LinearEOperator final : public EOperator
    {
    public:
        explicit LinearEOperator(double gain);
        double apply(double x) const override;

    private:
        double k_;
    };
}

#endif // ECT_SDK_E_OPERATOR_HPP
