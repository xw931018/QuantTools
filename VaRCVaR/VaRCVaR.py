import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

class VaR:
    def __init__(self, F, X, alpha=0.95):
        self.F = F
        self.X = X
        self.alpha = alpha

    def MC(self, gammaN, Psi=None, alpha=0.95):
        X = self.X
        if Psi is None:
            Psi = lambda x: x
        N = len(self.X)
        H1 = lambda xi, x: (1 - 1 / (1 - self.alpha) * (self.F(x) >= xi))
        H2 = lambda xi, C, x: (C - xi - 1 / (1 - self.alpha) * (Psi(self.F(x)) - xi) * (self.F(x) >= xi))
        self.H1 = H1
        self.H2 = H2
        xi = 0
        C = 0
        xi_bar = xi
        C_bar = C
        for n in np.arange(N):
            C = C - 1 / (n + 1) * H2(xi, C, X[n])
            xi = xi - gammaN[n] * H1(xi, X[n])
            C_bar = C_bar - 1 / (n + 1) * (C_bar - C)
            xi_bar = xi_bar - 1 / (n + 1) * (xi_bar - xi)
        return xi_bar, C_bar

    def MC_IS_Gaussian(self, gammaN, Psi=None, alpha=0.95):
        X = self.X
        if Psi is None:
            Psi = lambda x: x
        L1 = lambda xi, theta, x: np.exp(-0.5 * np.dot(x, x)) * (
        1 - 1 / (1 - alpha) * (self.F(x + theta) >= xi) * np.exp((-np.dot(theta, theta) - 2 * np.dot(theta, x)) / 2))
        L2 = lambda x1, C, mu, x: C - xi - 1 / (1 - alpha) * (Psi(self.F(x + mu)) - xi) * (
        self.F(x + mu) >= xi) * np.exp((-np.dot(mu, mu) - 2 * np.dot(mu, x)) / 2)
        L3 = lambda xi, theta, x: (self.F(x - theta) >= xi) * (2 * theta - x)
        L4 = lambda xi, mu, x: np.exp(-np.dot(mu, mu)) / (1 + xi ** 2 + 110) * (Psi(self.F(x - mu)) - xi) ** 2 * (
        Psi(self.F(x - mu)) - xi >= 0) * (2 * mu - x)
        self.L1 = L1;
        self.L2 = L2;
        self.L3 = L3;
        self.L4 = L4
        H1_hat = lambda xi, x, alpha: (1 - 1 / (1 - alpha) * (self.F(x) >= xi))
        N = len(self.X)
        M = int(N / 1000)
        alphas = [0.5 * (n <= M / 3) + 0.8 * (n > M / 3 and n <= 2 * M / 3) + alpha * (n > 2 * M / 3) for n in
                  np.arange(M)]
        xi = 0
        C = 0
        theta = 0
        mu = 0
        for n in np.arange(M):
            theta = theta - gammaN[n] * L3(xi, theta, X[n])
            mu = mu - gammaN[n] * L4(xi, mu, X[n])
            xi = xi - gammaN[n] * H1_hat(xi, X[n], alphas[n])
        xi_bar = xi
        C_bar = C
        for n in np.arange(N):
            xi = xi - gammaN[n] * L1(xi, theta, X[n])
            C = C - gammaN[n] * L2(xi, C, mu, X[n])
            theta = theta - gammaN[n] * L3(xi, theta, X[n])
            mu = mu - gammaN[n] * L4(xi, mu, X[n])
            C_bar = C_bar - 1 / (n + 1) * (C_bar - C)
            xi_bar = xi_bar - 1 / (n + 1) * (xi_bar - xi)
        return xi_bar, C_bar