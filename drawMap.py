import numpy as np
import matplotlib.pyplot as plt
import math
import os

##################################### Global Variable #################################
# Data points
dataBound = None
dataCent = None
dataCust = None

# Boundery limit
x_max = 50.0
x_min = 0.0
y_max = 50.0
y_min = 0.0
# final solution
best1stCenter = None

##################################### Function Define #################################
# rotate a point in a 2D plane
def Rotate(origin,target,degree):
	result = [0,0]
	o_x = float(origin[0])
	o_y = float(origin[1])
	t_x = float(target[0])
	t_y = float(target[1])
	d_x = t_x - o_x # delta x of origin & target
	d_y = t_y - o_y # delta y of origin & target
	radius = (degree/180.0)*math.pi # math.cos/math.sin need 'radius' not 'degree' as parameter
	result[0] = o_x + math.cos(radius)*d_x - math.sin(radius)*d_y
	result[1] = o_y + math.sin(radius)*d_x + math.cos(radius)*d_y
	return result
# Find first layer's center
def Center_find1stLayerCenter(candidates):
	bestCenter = None
	bestCost = -1
	for cenerCand in candidates:
		cenerCand.split()
		cx = float(cenerCand[0])
		cy = float(cenerCand[1])
		totalDist = 0
		for boundary in dataBound[1:]:
			boundary = boundary.split()
			bx = float(boundary[0])
			by = float(boundary[1])
			totalDist += math.sqrt((abs(cx-bx)**2 + abs(cy-by)**2))
		average = totalDist / len(dataBound[1:])
		
		totalDist = 0
		for boundary in dataBound[1:]:
			boundary = boundary.split()
			bx = float(boundary[0])
			by = float(boundary[1])
			totalDist += abs(math.sqrt((abs(cx-bx)**2 + abs(cy-by)**2)) - average)
			
		#print(cenerCand + ' ' + str(totalDist))
		if bestCost == -1 or totalDist < bestCost:
			bestCost = totalDist
			bestCenter = cenerCand
	return bestCenter.split()

# Read data points from files
def Read_file():
	global dataBound,dataCent,dataCust
	# open file (file path may use '/' or '\\')
	curPath = os.path.dirname(os.path.abspath(__file__))
	fileBound = open(curPath+"/boundaries.txt", "r")
	fileCust = open(curPath+"/customers.txt", "r")
	fileCent = open(curPath+"/centers.txt", "r")
	# read boundaries/centers/customers data points from file
	dataBound = fileBound.readlines()
	fileBound.close()
	dataCent = fileCent.readlines()
	fileCent.close()
	dataCust = fileCust.readlines()
	fileCust.close()

# Draw map for visualization
def Draw_map():
	global best1stCenter
	# plot boundary points
	count = int(dataBound[0])
	for boundaries in dataBound[1:]:
		boundaries = boundaries.split()
		x = float(boundaries[0])
		y = float(boundaries[1])
		plt.plot(x, y, 'k.') # k.: black point
		
	# plot customer points
	count = int(dataCust[0])	
	for customers in dataCust[1:]:
		customers = customers.split()
		x = float(customers[0])
		y = float(customers[1])
		plt.plot(x, y, 'b.') # b.: blue point

	# plot best 1st layer center
	count = int(dataCent[1])
	best1stCenter = Center_find1stLayerCenter(dataCent[2:2+count])
	plt.plot(float(best1stCenter[0]), float(best1stCenter[1]), 'r.') # r.: red point
	
	# plot 1st layer districting line
	rotate_degree = 0.0
	m_I = 3
	district_angle = 360.0 / m_I
	district_points = [[float(best1stCenter[0])+50.0,float(best1stCenter[1])]]
	district_points[0] = Rotate(best1stCenter,district_points[0],rotate_degree)
	for i in range(m_I-1):
		district_points.append(Rotate(best1stCenter,district_points[i],district_angle))
	for i in range(len(district_points)):
		x_1 = min(x_max,max(x_min,float(best1stCenter[0])))
		x_2 = min(x_max,max(x_min,district_points[i][0]))
		y_1 = min(y_max,max(y_min,float(best1stCenter[1])))
		y_2 = min(y_max,max(y_min,district_points[i][1]))
		# plot a line: plot([x_1,x_2],[y_1,y_2])
		plt.plot([x_1,x_2],[y_1,y_2],'k') # k: black line
	# show graph
	plt.xticks(np.arange(0,x_max,10))
	plt.show()	

##################################### Main Function #################################
Read_file()
Draw_map()










































































# plot center candidates points
'''attr = dataCent[0].split()
dataIdx = 1
layerIdx = 1
divides = int(attr[0])
rotate = int(attr[1])
layer = int(attr[2])

if layer == 2:
	divides += 1
	
for div in range(divides):
	count = int(dataCent[dataIdx])
	dataIdx += 1
	for centers in dataCent[dataIdx:dataIdx+count]:
		centers = centers.split()
		x = float(centers[0])
		y = float(centers[1])
		plt.plot(x, y, 'r.')
	dataIdx += count'''
	