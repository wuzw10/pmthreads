from output import * 
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
from scipy.stats.mstats import gmean
from scipy import mean 

#convert from BYTES to MB 
dram /= 1048576
nvm /= 1048576


fig = plt.figure()
ax = fig.add_subplot(111)
ax.plot(ticks, dram, marker='x', linewidth=1, alpha=0.9, label='volatile writes')
ax.plot(ticks, nvm, marker='o', linewidth=1, alpha=0.9, label='durable writes')
ax.legend(loc=1, ncol=1)
ax.set_title('figure-8: ' + str(delta) + 'ms')
ax.set_xlabel("time ticks (ms)")
ax.set_ylabel("Volume of data (MB)")
fig.savefig(figname)
print "Successfully generated " + figname 
saved  = dram-nvm
print '(Table-2 Delta=', delta, '):', mean(saved),'MB'
