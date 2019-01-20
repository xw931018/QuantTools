#include <algorithm>

class PayOff {
    public:
        PayOff() {};
        virtual ~PayOff() {};

        virtual double operator() (const double& spot) const = 0;
        virtual double Strike() const = 0;
};

class PayOffStrike : public PayOff {
    protected:
        double strike_;
    public:
        PayOffStrike() {}
        virtual ~PayOffStrike() {};
        virtual double operator() (const double& spot) const = 0;
        virtual double Strike() const { return strike_; };
};

class PayOffCall : public PayOffStrike {

    public:
        PayOffCall(const double& strike);
        virtual ~PayOffCall() {};
        virtual double operator() (const double& spot) const;
};

class PayOffPut : public PayOffStrike {

    public:
        PayOffPut(const double& strike);
        virtual ~PayOffPut() {};
        virtual double operator() (const double& spot) const;
};

class PayOffDigitalCall : public PayOffStrike {

    public:
        PayOffDigitalCall(const double& strike);
        virtual ~PayOffDigitalCall() {};
        virtual double operator() (const double& spot) const;
};

class PayOffDigitalPut : public PayOffStrike {

    public:
        PayOffDigitalPut(const double& strike);
        virtual ~PayOffDigitalPut() {};
        virtual double operator() (const double& spot) const;
};