import numpy as np
import matplotlib.pyplot as plt
import math
import os

##################################### Global Variable #################################
# Constant
LENGTH_OF_MAP = 20000
INTERVAL = 200
# Parameter
m_I = 4
m_O = 3
theta = 30
# Data points
dataBound = None
dataCent = None
dataCust = None

# final solution
best1stCenter = None

##################################### Function Definition #################################
# Find degree between center and customer
def Find_angle(center,customer):
	#atan2(y,x)
	delta_y = customer[1] - center[1]
	delta_x = customer[0] - center[0]
	deg = math.atan2(delta_y, delta_x)*180/math.pi
	if(deg < 0):
		deg += 360
	return deg
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
def Center_find1stLayerCenter():
	result = [0,0]
	result[0] = LENGTH_OF_MAP / 2.0
	result[1] = LENGTH_OF_MAP / 2.0
	return result
# Find 1st districting lines
def find1stLayerRotDeg(m_I,theta,best1stCenter):
	return 0
def find1stLayerDistrictPoint():
	district_angle = 360.0 / m_I
   	##############NOT DONE YET############################################################
   	#best_rotate_degree = find1stLayerRotDeg(m_I,theta,best1stCenter)					 #
	best_rotate_degree = 90									     #
	######################################################################################
	center = Center_find1stLayerCenter()
	end_points = [[float(center[0])+LENGTH_OF_MAP,float(center[1])]]
	end_points[0] = Rotate(center,end_points[0], best_rotate_degree)
	result = []
	for i in range(m_I-1):
		end_points.append(Rotate(center,end_points[i],district_angle))
	for i in range(0,len(end_points)):
		tmp_list = []
		result.append(tmp_list)
		delta_x = end_points[i][0] - center[0]
		delta_y = end_points[i][1] - center[1]
		slope = delta_y / delta_x
		deg = math.atan2(delta_y, delta_x)*180/math.pi
		if(deg < 0):
			deg += 360
		print(deg)
		# To count how many points we should draw on the line
		counter = int(math.sqrt(delta_x**2 + delta_y**2)) // INTERVAL # integer division
		result[i].append([center[0], center[1]])
		for j in range(counter): # counter: length of districting line / INTERVAL
			# if deg ~= 90, (return value of atan2 function return value might be 89.9999/90.999)
			#slope will be inf
			if(abs(deg-90) < 1):
				tmp_x = center[0]
				tmp_y = result[i][j-1][1] + INTERVAL
			# if deg ~= 270(return value of atan2 function return value might be 269.9999/270.999)
			# slope will be -inf
			elif(abs(deg-270) < 1):
				tmp_x = center[0]
				tmp_y = result[i][j-1][1] - INTERVAL
			# deg != 90 && deg != 270
			# if delta_x < 0, it means end point  is on the left of center
			# otherwise, end point is on the right of center
			else:
				if(delta_x < 0):
					tmp_x = result[i][j-1][0] - INTERVAL
					tmp_y = result[i][j-1][1] - INTERVAL*slope
				else:
					tmp_x = result[i][j-1][0] + INTERVAL
					tmp_y = result[i][j-1][1] + INTERVAL*slope	
			if(tmp_x < 0 or tmp_x > LENGTH_OF_MAP or tmp_y < 0 or tmp_y > LENGTH_OF_MAP):
				break
			result[i].append([tmp_x,tmp_y])
	return result
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
	# convert (x,y) from string to float, e.g.: ('10','10') ---> (10,10)
	for i in range(0,len(dataBound)):
		dataBound[i] = dataBound[i].split()
		dataBound[i][0] = float(dataBound[i][0])
		dataBound[i][1] = float(dataBound[i][1])
	for i in range(0,len(dataCust)):
		dataCust[i] = dataCust[i].split()
		dataCust[i][0] = float(dataCust[i][0])
		dataCust[i][1] = float(dataCust[i][1])
# Draw map for visualization
def Draw_map():
	global best1stCenter, theta
	# plot boundary points
	for point in dataBound[1:]:
		plt.plot(point[0], point[1], 'k.') # k.: black point
		
	# plot customer points	
	for point in dataCust[1:]:
		plt.plot(point[0], point[1], 'b.') # b.: blue point

	# plot best 1st layer center
	best1stCenter = Center_find1stLayerCenter()
	plt.plot(best1stCenter[0], best1stCenter[1], 'r.') # r.: red point
	
	# plot 1st layer districting line
	district_points = find1stLayerDistrictPoint()
	for i in range(len(district_points)):
		for j in range(len(district_points[i])):
			plt.plot(district_points[i][j][0], district_points[i][j][1], 'g.')
	
	# show graph
	plt.xticks(np.arange(0,LENGTH_OF_MAP+1,LENGTH_OF_MAP/10))
	plt.yticks(np.arange(0,LENGTH_OF_MAP+1,LENGTH_OF_MAP/10))
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
	