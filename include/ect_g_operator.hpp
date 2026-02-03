#ifndef ECT_SDK_G_OPERATOR_HPP
#define ECT_SDK_G_OPERATOR_HPP

namespace ect::sdk
{
    class GOperator
    {
    public:
        virtual ~GOperator() = default;
        virtual double apply(double delta) const = 0;
    };

    class LinearGOperator final : public GOperator
    {
    public:
        LinearGOperator(double gain, double u_min, double u_max);
        double apply(double delta) const override;

    private:
        double k_;
        double u_min_;
        double u_max_;
    };
}

#endif // ECT_SDK_G_OPERATOR_HPP
