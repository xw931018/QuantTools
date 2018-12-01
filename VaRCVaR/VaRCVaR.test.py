import numpy as np

import VaRCVaRUtils as utils
from OptionPricing import put

r = 0.05; K = 110; T = 1; S0 = 100; sigma = 0.2

N = 100
P0 = put(S0, r, sigma, T, K)
F = lambda x:(K - S0*np.exp((r-sigma**2/2)*T + sigma*np.sqrt(T)*x))*((K - S0*np.exp((r-sigma**2/2)*T + sigma*np.sqrt(T)*x))>=0)-np.exp(-r*T)*P0

utils.calculate1D(F, N = 1000, repeats = 100, usingIS = False, Psi = None, alpha = 0.95)