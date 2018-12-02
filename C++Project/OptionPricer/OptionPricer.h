class Option {
    public:
        Option(double t, double S0, double S, double K, double T, double r, double d, double sigma);
        virtual double PayOff(double S) = 0;

        double Price();

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
        double d1_;
        double d2_;

        void MonteCarloPrice(long N);
};

class CallOption: public Option {
    public:
        CallOption(double t, double S0, double S, double K, double T, double r, double d, double sigma);
        double PayOff(double S);

    private:
        int option_price_;
};

class PutOption: public Option {
    public:
        PutOption(double t, double S0, double S, double K, double T, double r, double d, double sigma);
        double PayOff(double S);

    private:
        int option_price_;
};