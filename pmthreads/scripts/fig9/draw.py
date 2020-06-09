from output import * 
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
from scipy.stats.mstats import gmean
from scipy import mean 

fig = plt.figure()
ax = fig.add_subplot(111)
data_to_plot = []
for t in [1, 2, 4, 8, 10 ]:
	data[t] *= 1000 #convert to us
	data_to_plot.append(data[t])
ax.boxplot(data_to_plot, vert=False, whis=0.75, showfliers=False)
ax.set_title('fig9')
ax.set_yticklabels(['1', '2', '4', '8', '10'])
ax.set_xlabel("time (us)")
ax.set_ylabel("Numbef of Memcached server threads.")
ax.grid(linewidth=0.5, alpha=0.4, axis='both')
fig.savefig(figname)
print "Successfully generated " + figname 
for t in [1, 2, 4, 8, 10 ]:
	print t, 'threads:','average time', mean(data[t]), 'us'


