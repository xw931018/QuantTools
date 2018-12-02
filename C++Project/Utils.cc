#include <cmath>
#include<random>
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