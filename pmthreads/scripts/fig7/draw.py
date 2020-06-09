from output import * 
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt
import numpy as np

fig = plt.figure()
ax = fig.add_subplot(111)
configs=data.keys()
markers=['+', 'o', 'x', '*', 'v', 'd', '3', '4']
i=0
for conf in configs:
	ax.plot(nthreads, data[conf], marker=markers[i],  linewidth=1, alpha=0.9, label=conf)
	i+=1
ax.legend(loc=2, ncol=2)
ax.set_title(figname)
ax.set_xlabel("#threads")
ax.set_ylabel("Throughput " + unit)
fig.savefig(figname)

print 'Successfully generated ' + figname
