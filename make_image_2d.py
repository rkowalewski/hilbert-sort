import numpy as np
import matplotlib.pyplot as plt
import sys

#N = 50
#x = np.random.rand(N)
#y = np.random.rand(N)
#
#plt.scatter(x, y)
#plt.show()

points = []

line = sys.stdin.readline()
while line:
    print line,
    point = line.split()
    points.append(map(int,point))
    line = sys.stdin.readline()

data = np.array(points)
x,y = data.T

npts = len(points)

for i in range(npts-1):
    plt.plot((points[i][0], points[i+1][0]), (points[i][1], points[i+1][1]))

plt.scatter(x, y)
plt.show()
