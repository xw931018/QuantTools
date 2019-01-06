class Option {
    public:
        Option();
        Option(double t, double S0, double S, double K, double T, double r, double d, double sigma);
        virtual double PayOff(const double& S) const = 0;
        virtual double ImpliedVolFitter(double, double) = 0;

        const double& Price() const;

        double interest_rate_;
        double price_;
        double price_mc_;
        double t_;
        double spot_0_;
        double spot_;
        double strike_;
        double maturity_;
        double dividend_;
        double vol_;
        double vol_implied_;
        double d1_;
        double d2_;

        void MonteCarloPrice(long N);
};


class CallOption: public Option {
    public:
        CallOption();
        CallOption(double t, double S0, double S, double K, double T, double r, double d, double sigma);
        double PayOff(const double& S) const;
        double ImpliedVolFitter(double, double);

    private:
        int option_price_;
};

class PutOption: public Option {
    public:
        PutOption();
        PutOption(double t, double S0, double S, double K, double T, double r, double d, double sigma);
        double PayOff(const double& S) const;
        double ImpliedVolFitter(double, double);

    private:
        int option_price_;
};