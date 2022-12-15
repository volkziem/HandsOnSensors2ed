# matplotlib_plotter.py, V. Ziemann, 221004
import numpy as np
import matplotlib.pyplot as plt
d=np.loadtxt('/home/pi/A0.dat',delimiter=' ')
plt.plot(d[:,0]-d[0,0],d[:,1])
plt.xlabel('Time [s]')
plt.ylabel('A0')
plt.show()
