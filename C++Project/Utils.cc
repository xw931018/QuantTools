#include <cmath>
#include <random>
#include <vector>
#include "Utils.h"

using namespace std;

double normalCFD(double value)
{
    return 0.5 * erfc(-value * M_SQRT1_2);
}

double stockPrice(double S0, double t, double T, double r, double d, double sigma) {
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<double> norm(0, 1);
    double z = norm(gen);
    return S0*exp((r- d - pow(sigma, 2)/2)*(T-t) + sigma*sqrt(T-t)*z);
}

GeometricBrownianPrice::GeometricBrownianPrice(double S0, double t, double T, double r, double d, double sigma) :
S0_(S0), t_(t), T_(T), r_(r), d_(d), sigma_(sigma) {}

vector<double>& GeometricBrownianPrice::operator() (const long& N) {
    prices_.resize(0);
    for (long i = 0; i < N; i++) {
        prices_.push_back(stockPrice(S0_, t_, T_, r_, d_, sigma_));
    }
    return prices_;
}