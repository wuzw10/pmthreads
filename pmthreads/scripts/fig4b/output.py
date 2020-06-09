import numpy as np
data={}
apps = ["histogram", "kmeans", "linear_regression", "matrix_multiply", "pca", "reverse_index", "string_match", "word_count", ]
figname='/tmp/phoenix.pdf'
data["pthread"] = np.array([0.210, 5.969, 0.088, 23.380, 3.575, 2.913, 0.166, 1.058, ])
data["pmthreads_i"] = np.array([1.935, 6.666, 1.198, 23.589, 3.681, 2.998, 1.384, 1.146, ])
data["pmthreads_p"] = np.array([0.211, 6.974, 0.097, 23.426, 3.680, 8.820, 0.168, 1.492, ])
data["atlas"] = np.array([4.202, 6.045, 3.316, 23.859, 3.940, 27.273, 2.761, 19.229, ])
data["nvthread"] = np.array([0.273, 11.161, 0.163, 23.702, 3.897, 62.806, 0.233, 1.199, ])
