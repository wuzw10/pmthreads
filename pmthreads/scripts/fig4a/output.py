import numpy as np
data={}
apps = ["blackscholes", "canneal", "dedup", "ferret", "streamcluster", "swaptions", ]
figname='/tmp/parsec.pdf'
data["pthread"] = np.array([25.944, 26.697, 0.280, 48.258, 1.005, 1.593, ])
data["pmthreads_i"] = np.array([25.038, 32.970, 0.236, 76.092, 1.076, 4.195, ])
data["pmthreads_p"] = np.array([24.729, 42.953, 0.395, 65.887, 1.006, 1.822, ])
data["atlas"] = np.array([26.931, 107.340, 0.405, 102.221, 2.643, 8.475, ])
data["nvthread"] = np.array([26.247, 49.552, 28.573, 249.308, 3.423, 1.722, ])
