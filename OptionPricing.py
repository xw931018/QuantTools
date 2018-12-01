import numpy as np
from scipy.stats import norm

from abc import ABCMeta, abstractmethod


def st(z, s0, r, sigma, T):
    return s0 * np.exp((r - sigma ** 2 / 2) * T + sigma * np.sqrt(T) * z)


s0 = 80;
r = 0.1;
sigma = 0.2;
T = 5;
K = 100


def call(s0, r, sigma, T, K):
    d1 = (np.log(s0 / (K * np.exp(-r * T))) + sigma ** 2 / 2 * T) / (sigma * np.sqrt(T))
    d2 = d1 - sigma * np.sqrt(T)
    return s0 * norm.cdf(d1) - K * np.exp(-r * T) * norm.cdf(d2)


def put(s0, r, sigma, T, K):
    d1 = (np.log(s0 / (K * np.exp(-r * T))) + sigma ** 2 / 2 * T) / (sigma * np.sqrt(T))
    d2 = d1 - sigma * np.sqrt(T)
    return K * np.exp(-r * T) * norm.cdf(-d2) - s0 * norm.cdf(-d1)


class Option(object):
    __metaclass__ = ABCMeta

    def __init__(self, s0, r, sigma, K, T, name=None):
        if name is not None:
            self.name = name
        self.r = r
        self.sigma = sigma
        self.K = K
        self.T = T
        self.s0 = s0
        self.d1 = (np.log(s0 / (K * np.exp(-r * T))) + sigma ** 2 / 2 * T) / (sigma * np.sqrt(T))
        self.d2 = self.d1 - sigma * np.sqrt(T)
        pass

    @abstractmethod
    def payoff(self, S):
        print("Abstract Option class payoff function")
        pass

    @abstractmethod
    def formula(self):
        print("Abstract Option class pricing formula")
        pass

    def monte_carlo(self, M, alpha=0.05):
        randn = np.random.normal(size=M)
        stock_price = st(randn, self.s0, self.r, self.sigma, self.T)
        stock_price_minus = st(-randn, self.s0, self.r, self.sigma, self.T)
        intrinsic_mc = np.exp(-self.r * self.T) * self.payoff(stock_price)
        intrinsic_mc_anti = np.exp(-self.r * self.T) * self.payoff(stock_price_minus)
        intrinsic = (intrinsic_mc + intrinsic_mc_anti) / 2
        price_mc = np.mean(intrinsic)
        std_mc = np.std(intrinsic)
        price_mc_no_anti = np.mean(intrinsic_mc)
        std_mc_no_anti = np.std(intrinsic_mc)
        qalpha = norm.ppf(1 - alpha / 2)
        delta_anti = std_mc / np.sqrt(M) * qalpha
        delta_no_anti = std_mc_no_anti / np.sqrt(M) * qalpha
        return dict({"SampleSize": M,
                     "Price Antithetic Var": price_mc,
                     "Std Antithetic Var": std_mc,
                     "Interval Antithetic Var": (price_mc - delta_anti, price_mc + delta_anti),
                     "Price No Anti": price_mc_no_anti,
                     "Std No Anti": std_mc_no_anti,
                     "Interval No Anti": (price_mc_no_anti - delta_no_anti, price_mc_no_anti + delta_no_anti)})

    def delta_monte_carlo(self, M, alpha=0.05):
        randn = np.random.normal(size=M)
        stock_price = st(randn, self.s0, self.r, self.sigma, self.T)
        stock_price_minus = st(-randn, self.s0, self.r, self.sigma, self.T)
        intrinsic_mc = np.exp(-self.r * self.T) * self.payoff(stock_price) * randn * np.sqrt(self.T) / (
        self.s0 * self.sigma * self.T)
        intrinsic_mc_anti = np.exp(-self.r * self.T) * self.payoff(stock_price_minus) * (-randn) * np.sqrt(self.T) / (
        self.s0 * self.sigma * self.T)
        intrinsic = (intrinsic_mc + intrinsic_mc_anti) / 2
        delta_mc = np.mean(intrinsic)
        return delta_mc


class Call(Option):
    def payoff(self, S):
        if isinstance(S, int):
            return np.array([np.max([S - self.K, 0])])
        else:
            return np.array([np.max([s - self.K, 0]) for s in S])

    def formula(self):
        return self.s0 * norm.cdf(self.d1) - self.K * np.exp(-self.r * self.T) * norm.cdf(self.d2)

    def delta(self):
        return norm.cdf(self.d1)


class Put(Option):
    def payoff(self, S):
        if isinstance(S, int):
            return np.array([np.max([self.K - S, 0])])
        else:
            return np.array([np.max([self.K - s, 0]) for s in S])

    def formula(self):
        return -self.s0 * norm.cdf(-self.d1) + self.K * np.exp(-self.r * self.T) * norm.cdf(-self.d2)

    def delta(self):
        return -norm.cdf(-self.d1)