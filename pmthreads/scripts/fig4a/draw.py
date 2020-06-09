from output import * 
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
from scipy.stats.mstats import gmean

configs=data.keys()
configs.remove('pthread')
apps.append('geomean')
print apps
expected_len=len(apps)
np.set_printoptions(formatter={'float': '{: 0.3f}'.format})
for conf in configs:
    data[conf] = data[conf]/data['pthread']
    data[conf] = np.append(data[conf], gmean(data[conf]))
    assert(data[conf].size==expected_len)
    print conf,  data[conf]

fig = plt.figure()
ax = fig.add_subplot(111)
markers=['+', 'o', 'x', '*', 'v', 'd']
colors=['grey', 'green', 'blue', 'black']
bar_width = 0.2
r = np.arange(len(apps))
i = 0
for conf in configs:
	r = [x + bar_width for x in r]
	ax.bar(r, data[conf], width=bar_width, label=conf, color=colors[i])
	i+=1
ax.legend(loc=2, ncol=2)
plt.xticks([r + 2*bar_width for r in range(len(apps))], [str[0:3]+'.' for str in apps])
ax.set_ylim(top=12)
ax.set_title("parsec")
#ax.set_xlabel("#threads")
ax.set_ylabel("Slowdown over pthread")
fig.savefig(figname)
print "Successfully generated " + figname 
