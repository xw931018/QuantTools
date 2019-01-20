#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include "OptionPricer.h"
#include "../Utils.h"
#include "../NonlinearSolver/NonlinearSolver.h"

using namespace std;

Option::Option(PayOff* _payoff) : payoff(_payoff) { strike_ = payoff->Strike(); }

void Option::SetParameters(double t, double S, double T, double r, double d, double sigma) {
    t_ = t; spot_ = S; maturity_ = T; dividend_ = d; vol_ = sigma;
    interest_rate_ = r;
    double K = strike_;
    d1_ = (log(S/K) + (r - d + sigma*sigma/2)*(T-t))/sigma/sqrt(T-t);
    d2_ = (log(S/K) + (r - d - sigma*sigma/2)*(T-t))/sigma/sqrt(T-t);
}

double Option::ClosedFormPriceVanilla(double t, double S, double T, double r, double d, double sigma) {
    return 0;
}

const double& Option::Price() const {
    return price_;
}

double Option::MonteCarloPrice(const vector<double>& stock_prices) {
    long size = static_cast<long>(stock_prices.size());
    double sum = 0;
    for ( long i = 0; i < size; i++ ) {
        sum += (*payoff)(stock_prices[i]);
    }
    price_mc_ = sum / size;
    return price_mc_;
}


CallOption::CallOption(PayOff* payoff):
    Option(payoff) {}

double CallOption::ClosedFormPriceVanilla(double t, double S, double T, double r, double d, double sigma) {
    SetParameters(t, S, T, r, d, sigma);
    double K = strike_;
    price_ = S*exp(-d*(T-t))*normalCFD(d1_) - K*exp(-r*(T-t))*normalCFD(d2_);
    return price_;
}

double CallOption::ImpliedVolFitter(double CallPrice, double initial, double t, double S, double T, double r, double d) {
    double K = strike_;
    auto f = [S, K, r, d, T, t, CallPrice] (double sigma) {
        double d1 = (log(S/K) + (r - d + sigma*sigma/2)*(T-t))/sigma/sqrt(T-t);
        double d2 = (log(S/K) + (r - d - sigma*sigma/2)*(T-t))/sigma/sqrt(T-t);
        double p = S*exp(-d*(T-t))*normalCFD(d1) - K*exp(-r*(T-t))*normalCFD(d2);
        return p - CallPrice;
        };
    SteffensenSolver solver(f, initial);
    double impVol = solver.solve();
    return impVol;
}


PutOption::PutOption(PayOff* payoff):
    Option(payoff) {}

double PutOption::ClosedFormPriceVanilla(double t, double S, double T, double r, double d, double sigma) {
    SetParameters(t, S, T, r, d, sigma);
    double K = strike_;
    price_ = K*exp(-r*(T-t))*normalCFD(-d2_) - S*exp(-d*(T-t))*normalCFD(-d1_);
    return price_;
}

double PutOption::ImpliedVolFitter(double PutPrice, double initial, double t, double S, double T, double r, double d) {
    double K = strike_;
    auto f = [S, K, r, d, T, t, PutPrice] (double sigma) {
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


DigitalOption::DigitalOption(PayOff* payoff):
    Option(payoff) {}

int main() {
    //Test Call PayOff
    PayOffCall* payCall = new PayOffCall(60);
    PayOffPut * payPut  = new PayOffPut(60);
    CallOption call(payCall);
    PutOption  put(payPut);

    cout << "Call Closed Form: " << call.ClosedFormPriceVanilla(0, 59, 0.25, 0.067, 0.067, 0.1) << endl;
    cout << "Put Closed Form: " << put.ClosedFormPriceVanilla(0, 59, 0.25, 0.067, 0.067, 0.1) << endl;

    vector<double> prices(100, 0) ;
    cout << "Call Monte Carlo Price: " << call.MonteCarloPrice(prices) << endl;
    cout << "Put Monte Carlo Price: " << call.MonteCarloPrice(prices) << endl;

    cout << "Test call implied volatility fitter " << call.ImpliedVolFitter(4, 0.22, 0, 59, 0.25, 0.067, 0.067) << endl;
    cout << "Test put implied volatility fitter " << put.ImpliedVolFitter(4, 0.44, 0, 59, 0.25, 0.067, 0.067) << endl;

    delete payCall;
    delete payPut;
}


//int main() {
//    CallOption call1(0, 59, 59, 60, 0.25, 0.067, 0.067, 0.1);
//    DigitalOption digital(0, 60, 59, 60, 0.25, 0.067, 0.067, 0.1);
//
//    digital.MonteCarloPrice(10000);
//
//    cout << "Digital Option call price is " << digital.price_mc_ << endl;
//
//    cout << "Closed-form call option price is " << call1.Price() << endl;
//    PutOption put1(0, 59, 59, 60, 0.25, 0.067, 0.067, 0.1);
//    cout << "Closed-form put option price is " << put1.Price() << endl;
//    call1.MonteCarloPrice(10000);
//    cout << "Monte-Carlo Price for call option is " << call1.price_mc_ << endl;
//    put1.MonteCarloPrice(10000);
//    cout << "Monte-Carlo Price for put option is " << put1.price_mc_ << endl;
//    cout << "Test call implied volatility fitter " << call1.ImpliedVolFitter(4, 0.22) << endl;
//    cout << "Test put implied volatility fitter " << put1.ImpliedVolFitter(5, 0.44) << endl;
//    CallOption C1;
//    Option* C2 = &C1; //new PutOption();
//    const type_info& typeC1 = typeid(C1);
//    cout << "Human readable type " << typeC1.name() << endl;
//    if (typeid(C1) == typeid(*C2)) {
//        cout << "Type ID Equal ! " <<  endl;
//    } else {
//        cout << "Not Equal" << typeid(C2).name() << endl;
//    }
//
//}