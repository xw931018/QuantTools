#include "PayOff.h"
#include <vector>

using namespace std;

class Option {
    protected:
        PayOff* payoff;

    public:
        Option() {};
        Option(PayOff*);
        virtual ~Option() {};
        void SetParameters(double t, double S, double T, double r, double d, double sigma);
        virtual double ClosedFormPriceVanilla(double t, double S, double T, double r, double d, double sigma);

        const double& Price() const;

        double interest_rate_;
        double price_;
        double price_mc_;
        double t_;
        double spot_;
        double strike_;
        double maturity_;
        double dividend_;
        double vol_;
        double vol_implied_;
        double d1_;
        double d2_;

        double MonteCarloPrice(const vector<double>& stock_prices);
};


class CallOption: public Option {
    public:
        CallOption() {};
        CallOption(PayOff* payoff);
        virtual ~CallOption() {};
        virtual double ClosedFormPriceVanilla(double t, double S, double T, double r, double d, double sigma);
        double ImpliedVolFitter(double CallPrice, double initial, double t, double S, double T, double r, double d);
};

class PutOption: public Option {
    public:
        PutOption() {};
        PutOption(PayOff* payoff);
        virtual ~PutOption() {};
        virtual double ClosedFormPriceVanilla(double t, double S, double T, double r, double d, double sigma);
        double ImpliedVolFitter(double CallPrice, double initial, double t, double S, double T, double r, double d);
};

class DigitalOption: public Option {
    public:
        DigitalOption() {};
        DigitalOption(PayOff* payoff);
        virtual ~DigitalOption() {};
};