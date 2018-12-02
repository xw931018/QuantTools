from VaRCVaR import VaR
import numpy as np

def calculate1D(F, N, repeats=100, usingIS=False, Psi=None, alpha=0.95):
    print("Alpha is ", alpha, ",  Number of iterations: ", N, " Using IS method ? ", usingIS)
    Xi = []
    C = []
    gammaN = np.array([1 / (n ** (3 / 4) + 100) for n in (np.arange(N) + 1)])
    for i in range(repeats):
        X = np.random.normal(size=N)
        putVaR = VaR(F, X, alpha)
        if usingIS:
            xi, c = putVaR.MC_IS_Gaussian(gammaN, Psi, alpha)
        else:
            xi, c = putVaR.MC(gammaN, Psi, alpha)

        Xi.append(xi)
        C.append(c)
    print("Mean on VaR: ", np.mean(Xi))
    print("Variance on VaR: ", np.var(Xi))
    print("Mean on CVaR: ", np.mean(C))
    print("Variance on CVaR: ", np.var(C))
    print("------------------------------------------")