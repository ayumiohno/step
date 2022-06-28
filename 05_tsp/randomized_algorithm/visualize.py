import matplotlib.pyplot as plt
import numpy as np
import math
import sys
import matplotlib.collections as mc
import matplotlib.cm as cm

input_file_name = sys.argv[1]


points_num = 0
# x, y
points = []

with open(input_file_name, "r") as f:
    points_num = f.readline()
    print(points_num)
    points = [
        list(map(float, f.readline().split())) for _ in range(int(points_num))
    ]


    lines = [ [(points[i][0], points[i][1]), (points[(i + 1) % int(points_num)][0], points[(i+1)% int(points_num)][1])] for i in range(int(points_num))]

lc = mc.LineCollection(lines)
fig = plt.figure(figsize=(4,4))
ax = fig.add_subplot(aspect='1')
ax.add_collection(lc)
ax.autoscale()
plt.show()
