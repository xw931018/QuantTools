class Option {
    public:
        virtual double PayOff(double S) = 0;

        double Price();
        double price_;
};

class VanillaOption: public Option {
    public:
        VanillaOption(double t, double S, double K, double T, double r, double d, double sigma);
        virtual double PayOff(double S) = 0;
        double Price();

        double t_;
        double spot_;
        double strike_;
        double maturity_;
        double interest_rate_;
        double dividend_;
        double vol_;
        double d1_;
        double d2_;
};

class CallOption: public VanillaOption {
    public:
        CallOption(double t, double S, double K, double T, double r, double d, double sigma);
        double PayOff(double S);

    private:
        int option_price_;
};

class PutOption: public VanillaOption {
    public:
        PutOption(double t, double S, double K, double T, double r, double d, double sigma);
        double PayOff(double S);

    private:
        int option_price_;
};