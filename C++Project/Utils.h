#include <vector>

using namespace std;

double normalCFD(double value);

double stockPrice(double S0, double t, double T, double r, double d, double sigma);

class GeometricBrownianPrice {
    protected:
        double S0_;
        double t_;
        double T_;
        double r_;
        double d_;
        double sigma_;
        vector<double> prices_;
    public:
        GeometricBrownianPrice() {}
        GeometricBrownianPrice(double S0, double t, double T, double r, double d, double sigma);
        virtual ~GeometricBrownianPrice() {}
        vector<double>& operator() (const long& N);
};