import numpy as np
import matplotlib.pyplot as plt
import sys
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm

points = []

line = sys.stdin.readline()
while line:
    point = line.split()
    points.append(map(int,point))
    line = sys.stdin.readline()

data = np.array(points)
x,y,z = data.T


print points

npts = len(points)

fig = plt.figure()
ax = fig.gca(projection='3d')
cmap = cm.nipy_spectral

s = 1
for i in range(0, npts-s, s):
    ax.plot(x[i:i+s+1], y[i:i+s+1], z[i:i+s+1])

plt.tight_layout()
plt.show()
