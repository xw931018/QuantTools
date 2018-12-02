#include <iostream>
#include <cmath>
#include <algorithm>
#include "OptionPricer.h"

using namespace std;

double normalCFD(double value)
{
 return 0.5 * erfc(-value * M_SQRT1_2);
}

VanillaOption::VanillaOption(double t, double S, double K, double T, double r, double d, double sigma):
    t_(t), spot_(S), strike_(K), maturity_(T), interest_rate_(r), dividend_(d), vol_(sigma) {
    d1_ = (log(S/K) + (r - d + sigma*sigma/2)*(T-t))/sigma/sqrt(T-t);
    d2_ = (log(S/K) + (r - d - sigma*sigma/2)*(T-t))/sigma/sqrt(T-t);
    }

double VanillaOption::Price() {
    return price_;
}

void Option::MonteCarloPrice(int N) {
    double sum = 0;
    for (n = 0; n < N; n++) {

    }

}

CallOption::CallOption(double t, double S, double K, double T, double r, double d, double sigma):
    VanillaOption(t, S, K, T, r, d, sigma) {
        price_ = S*exp(-d*(T-t))*normalCFD(d1_) - K*exp(-r*(T-t))*normalCFD(d2_);
    }

double CallOption::PayOff(double S) {
    return std::max(S - strike_, 0.0);
}

PutOption::PutOption(double t, double S, double K, double T, double r, double d, double sigma):
    VanillaOption(t, S, K, T, r, d, sigma) {
        price_ = K*exp(-r*(T-t))*normalCFD(-d2_) - S*exp(-d*(T-t))*normalCFD(-d1_);

    }

double PutOption::PayOff(double S) {
    return std::max(strike_ - S, 0.0);
}


int main() {
    CallOption call1(0, 50, 100, 3, 0.05, 0.01, 0.1);
    cout << call1.Price() << endl;
    PutOption put1(0, 0, 100, 3, 0.05, 0.01, 0.1);
    cout << put1.Price() << endl;
}