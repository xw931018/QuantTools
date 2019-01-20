#include "PayOff.h"
#include <algorithm>


// PayOffCall
PayOffCall::PayOffCall(const double& strike) {strike_ = strike; }

double PayOffCall::operator() (const double& spot) const {
    return std::max(spot - strike_, 0.0);
}

//PayOffPut
PayOffPut::PayOffPut(const double& strike) { strike_ = strike; }

double PayOffPut::operator() (const double& spot) const {
    return std::max(strike_ - spot, 0.0);
}

//PayOffDigitalCall
PayOffDigitalCall::PayOffDigitalCall(const double& strike) { strike_ = strike; }

double PayOffDigitalCall::operator() (const double& spot) const {
    if (spot >= strike_) {
        return 1;
    }
    return 0;
}


//PayOffDigitalPut
PayOffDigitalPut::PayOffDigitalPut(const double& strike) { strike_ = strike; }

double PayOffDigitalPut::operator() (const double& spot) const {
    if (spot < strike_) {
        return 1;
    }
    return 0;
}

