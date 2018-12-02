#include <iostream>
#include <cmath>
#include <algorithm>
#include "OptionPricer.h"
#include "../Utils.h"

using namespace std;

Option::Option(double t, double S0, double S, double K, double T, double r, double d, double sigma):
    t_(t), spot_0_(S0), spot_(S), strike_(K), maturity_(T), dividend_(d), vol_(sigma) {
    interest_rate_ = r;
    d1_ = (log(S/K) + (r - d + sigma*sigma/2)*(T-t))/sigma/sqrt(T-t);
    d2_ = (log(S/K) + (r - d - sigma*sigma/2)*(T-t))/sigma/sqrt(T-t);
    }

double Option::Price() {
    return price_;
}

void Option::MonteCarloPrice(long N) {
    double sum    = 0;
    double payOff = 0;
    double s      = 0;
    for (long n = 0; n < N; n++) {
        s = stockPrice(spot_0_, t_, maturity_, interest_rate_, dividend_, vol_);
        payOff = PayOff(s);
        sum += payOff * exp(-interest_rate_*(maturity_-t_));
    }
    price_mc_ = sum/N;
}

CallOption::CallOption(double t, double S0, double S, double K, double T, double r, double d, double sigma):
    Option(t, S0, S, K, T, r, d, sigma) {
        price_ = S*exp(-d*(T-t))*normalCFD(d1_) - K*exp(-r*(T-t))*normalCFD(d2_);
    }

double CallOption::PayOff(double S) {
    return std::max(S - strike_, 0.0);
}

PutOption::PutOption(double t, double S0, double S, double K, double T, double r, double d, double sigma):
    Option(t, S0, S, K, T, r, d, sigma) {
        price_ = K*exp(-r*(T-t))*normalCFD(-d2_) - S*exp(-d*(T-t))*normalCFD(-d1_);

    }

double PutOption::PayOff(double S) {
    return std::max(strike_ - S, 0.0);
}


//int main() {
//    CallOption call1(0, 120, 120, 100, 10, 0.05, 0.01, 0.1);
//    cout << "Closed-form call option price is " << call1.Price() << endl;
//    PutOption put1(0, 80, 80, 100, 3, 0.05, 0.01, 0.1);
//    cout << "Closed-form put option price is " << put1.Price() << endl;
//    call1.MonteCarloPrice(100);
//    cout << "Monte-Carlo Price for call option is " << call1.price_mc_ << endl;
//    put1.MonteCarloPrice(100);
//    cout << "Monte-Carlo Price for put option is " << put1.price_mc_ << endl;
//
//}