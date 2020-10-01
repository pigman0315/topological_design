import numpy as np
import matplotlib.pyplot as plt
import math
import os

##################################### Global Variable #################################
# Parameter
theta = 10
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
		cenerCand.split() # split x , y
		cx = float(cenerCand[0])
		cy = float(cenerCand[1])
		totalDist = 0
		for boundary in dataBound[1:]:
			boundary = boundary.split() # split x , y
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
			
		if bestCost == -1 or totalDist < bestCost:
			bestCost = totalDist
			bestCenter = cenerCand
	bestCenter = bestCenter.split()
	bestCenter[0] = float(bestCenter[0])
	bestCenter[1] = float(bestCenter[1])
	return bestCenter
# Find 1st districting
def find1stLayerRotDeg(m_I,theta,best1stCenter):
	global dataCust
	for k in range(0,int(360/m_I), theta):
		limits = [] #cos value of districting point
		for i in range(m_I):
			limits.append(math.cos(((k+(360/m_I)*i)/180.0)*math.pi))
		count = int(dataCust[0])
		for customers in dataCust[1:]:
			customers = customers.split()
			v1 = [float(customers[0])-best1stCenter[0],float(customers[1])-best1stCenter[1]]
			v0 = [1,0]
			cos_val = (v0[0]*v1[0]+v0[1]*v1[1])/(math.sqrt(v0[0]**2+v0[1]**2)*math.sqrt(v1[0]**2+v1[1]**2))
			print(cos_val)
	return 0

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
	global best1stCenter, theta
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
	plt.plot(best1stCenter[0], best1stCenter[1], 'r.') # r.: red point
	
	# plot 1st layer districting line
	m_I = 4
	district_angle = 360.0 / m_I
	rotate_degree = find1stLayerRotDeg(m_I,theta,best1stCenter)
	district_points = [[float(best1stCenter[0])+x_max,float(best1stCenter[1])]]
	district_points[0] = Rotate(best1stCenter,district_points[0],rotate_degree)
	for i in range(m_I-1):
		district_points.append(Rotate(best1stCenter,district_points[i],district_angle))
	for i in range(len(district_points)):
		x_1 = best1stCenter[0]
		x_2 = district_points[i][0]
		y_1 = best1stCenter[1]
		y_2 = district_points[i][1]
		# plot a line: plot([x_1,x_2],[y_1,y_2])
		plt.plot([x_1,x_2],[y_1,y_2],'k') # k: black line
	# show graph
	plt.xticks(np.arange(0,x_max+10,10))
	plt.yticks(np.arange(0,x_max+10,10))
	plt.axis('equal')
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
	