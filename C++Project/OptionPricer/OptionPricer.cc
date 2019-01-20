#include <iostream>
#include <cmath>
#include <algorithm>
#include "OptionPricer.h"
#include "../Utils.h"
#include "../NonlinearSolver/NonlinearSolver.h"

using namespace std;

Option::Option() {
    interest_rate_ = 0;
    t_ = 0;
    spot_ = 100;
    spot_0_ = 100;
    strike_ = 120;
    maturity_ = 1;
    dividend_ = 0;
    vol_ = 0.2;
}

Option::Option(double t, double S0, double S, double K, double T, double r, double d, double sigma):
    t_(t), spot_0_(S0), spot_(S), strike_(K), maturity_(T), dividend_(d), vol_(sigma) {
    interest_rate_ = r;
    d1_ = (log(S/K) + (r - d + sigma*sigma/2)*(T-t))/sigma/sqrt(T-t);
    d2_ = (log(S/K) + (r - d - sigma*sigma/2)*(T-t))/sigma/sqrt(T-t);
    }

const double& Option::Price() const {
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

CallOption::CallOption() : Option() {
}

CallOption::CallOption(double t, double S0, double S, double K, double T, double r, double d, double sigma):
    Option(t, S0, S, K, T, r, d, sigma) {
        price_ = S*exp(-d*(T-t))*normalCFD(d1_) - K*exp(-r*(T-t))*normalCFD(d2_);
    }

double CallOption::PayOff(const double &S) const{
    return std::max(S - strike_, 0.0);
}

double CallOption::ImpliedVolFitter(double CallPrice, double initial) {
    double S = spot_;
    double K = strike_;
    double r = interest_rate_;
    double d = dividend_;
    double T = maturity_;
    double t = t_;
    auto f = [S, K, r, d, T, t, CallPrice] (double sigma) {
//        double d1 = (log(spot_/strike_) + (interest_rate_ - dividend_ + sigma*sigma/2)*(maturity_-t_))/sigma/sqrt(maturity_-t_);
//        double d2 = (log(spot_/strike_) + (interest_rate_ - dividend_ - sigma*sigma/2)*(maturity_-t_))/sigma/sqrt(maturity_-t_);
//        double p = spot_*exp(-dividend_*(maturity_-t_))*normalCFD(d1) - strike_*exp(-interest_rate_*(maturity_-t_))*normalCFD(d2);
        double d1 = (log(S/K) + (r - d + sigma*sigma/2)*(T-t))/sigma/sqrt(T-t);
        double d2 = (log(S/K) + (r - d - sigma*sigma/2)*(T-t))/sigma/sqrt(T-t);
        double p = S*exp(-d*(T-t))*normalCFD(d1) - K*exp(-r*(T-t))*normalCFD(d2);
        return p - CallPrice;
        };
    SteffensenSolver solver(f, initial);
    double impVol = solver.solve();
    return impVol;
}

PutOption::PutOption() : Option() {}

PutOption::PutOption(double t, double S0, double S, double K, double T, double r, double d, double sigma):
    Option(t, S0, S, K, T, r, d, sigma) {
        price_ = K*exp(-r*(T-t))*normalCFD(-d2_) - S*exp(-d*(T-t))*normalCFD(-d1_);
    }



double PutOption::ImpliedVolFitter(double PutPrice, double initial) {
    double S = spot_;
    double K = strike_;
    double r = interest_rate_;
    double d = dividend_;
    double T = maturity_;
    double t = t_;
    auto f = [S, K, r, d, T, t, PutPrice] (double sigma) {
//        double d1 = (log(spot_/strike_) + (interest_rate_ - dividend_ + sigma*sigma/2)*(maturity_-t_))/sigma/sqrt(maturity_-t_);
//        double d2 = (log(spot_/strike_) + (interest_rate_ - dividend_ - sigma*sigma/2)*(maturity_-t_))/sigma/sqrt(maturity_-t_);
        double d1 = (log(S/K) + (r - d + sigma*sigma/2)*(T-t))/sigma/sqrt(T-t);
        double d2 = (log(S/K) + (r - d - sigma*sigma/2)*(T-t))/sigma/sqrt(T-t);

//        double p = - spot_*exp(-dividend_*(maturity_-t_))*normalCFD(-d1) + strike_*exp(-interest_rate_*(maturity_-t_))*normalCFD(-d2);
        double p = - S*exp(-d*(T-t))*normalCFD(-d1) + K*exp(-r*(T-t))*normalCFD(-d2);
        return p - PutPrice;
        };
    SteffensenSolver solver(f, initial);
    double impVol = solver.solve();
    return impVol;
}

double PutOption::PayOff(const double& S) const {
    return std::max(strike_ - S, 0.0);
}

DigitalOption::DigitalOption() : Option() {}

DigitalOption::DigitalOption(double t, double S0, double S, double K, double T, double r, double d, double sigma):
    Option(t, S0, S, K, T, r, d, sigma) {
}

double DigitalOption::PayOff(double const& S) const {
    if (S >= strike_) {
        return 1.0;
    } else {
        return 0;
    }
}

double DigitalOption::ImpliedVolFitter(double DigitalPrice, double initial) {
    return vol_;
}

int main() {
    CallOption call1(0, 59, 59, 60, 0.25, 0.067, 0.067, 0.1);
    DigitalOption digital(0, 60, 59, 60, 0.25, 0.067, 0.067, 0.1);

    digital.MonteCarloPrice(10000);

    cout << "Digital Option call price is " << digital.price_mc_ << endl;

    cout << "Closed-form call option price is " << call1.Price() << endl;
    PutOption put1(0, 59, 59, 60, 0.25, 0.067, 0.067, 0.1);
    cout << "Closed-form put option price is " << put1.Price() << endl;
    call1.MonteCarloPrice(10000);
    cout << "Monte-Carlo Price for call option is " << call1.price_mc_ << endl;
    put1.MonteCarloPrice(10000);
    cout << "Monte-Carlo Price for put option is " << put1.price_mc_ << endl;
    cout << "Test call implied volatility fitter " << call1.ImpliedVolFitter(4, 0.22) << endl;
    cout << "Test put implied volatility fitter " << put1.ImpliedVolFitter(5, 0.44) << endl;
    CallOption C1;
    Option* C2 = &C1; //new PutOption();
    const type_info& typeC1 = typeid(C1);
    cout << "Human readable type " << typeC1.name() << endl;
    if (typeid(C1) == typeid(*C2)) {
        cout << "Type ID Equal ! " <<  endl;
    } else {
        cout << "Not Equal" << typeid(C2).name() << endl;
    }

}