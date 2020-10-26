import numpy as np
import matplotlib.pyplot as plt
import math
import os
from shapely.geometry import Polygon
from shapely import wkt

##################################### Global Variable #################################
# Constant
LENGTH_OF_MAP = 20000
INTERVAL = 600
MAGIC_NUMBER = 100 # To avoid districting lines in 2nd layer are too short
# Parameter
# m_I = 4 --> degree = 90, m_I = 3 ---> degree = 120
# m_O = 3 --> degree = 90, m_O = 2 ---> degree = 120
# w = 1 ---> single layer, w = 2 ---> two layer
m_I = 3
m_O = 2
w = 2
best_rotate_degree_1st = 0

# Data points from files
dataBound = None
dataCent = None
dataCust = None

# Districting points
district_points_1st = None
district_points_2nd = None

# center
best1stCenter = None
best2ndCenter = []

##################################### Function Definition #################################
def show_info():
	print("*******************  Map Information  *****************")
	print("Side length of the map: ", LENGTH_OF_MAP,"(m)")
	print("Interval between points: ", INTERVAL,"(m)")
	print("Best 1st layer rotation degree: ", best_rotate_degree_1st , "(degree)")
	print("1-layer or 2-layer:", w)
	print("Number of inner district(m_I): ", m_I)
	print("Number of outter district(m_O): ", m_O)

# subfunction for out_of_bound to return the cross value of p1p2 line & p1p3 line 
def cross(p1,p2,p3):
	x1=p2[0]-p1[0]
	y1=p2[1]-p1[1]
	x2=p3[0]-p1[0]
	y2=p3[1]-p1[1]
	return x1*y2-x2*y1 
# check if the points out of bound
def out_of_bound(point, center, mode):
	district_angle = 360.0 / m_I
	if(point[0] < 0 or point[0] > LENGTH_OF_MAP or point[1] < 0 or point[1] > LENGTH_OF_MAP):
		return True
	if(mode == 2):
			# get endpoint of 1st layer districting lines
			end_points = []
			for i in range(m_I):
				end_points.append(district_points_1st[i][len(district_points_1st[i])-1])
			
			# check if two line segment intersect
			# It's an algorithm to find if two line segment intersect
			for i in range(len(end_points)):
				# line1 = p1_1 --- p1_2
				p1_1 = end_points[i]
				p1_2 = best1stCenter
				# line2 = p1_1 --- p1_2
				p2_1 = point
				p2_2 = center
				# step1: exclude lines whose max x/y is smaller than the other line's min x/y
				if(max(p1_1[0],p1_2[0]) >= min(p2_1[0],p2_2[0])
				and max(p2_1[0],p2_2[0]) >= min(p1_1[0],p1_2[0])
				and max(p1_1[1],p1_2[1]) >= min(p2_1[1],p2_2[1])
				and max(p2_1[1],p2_2[1]) >= min(p1_1[1],p1_2[1])):
					# cross product checking
					if(cross(p1_1,p1_2,p2_1)*cross(p1_1,p1_2,p2_2) <= 0
						and cross(p2_1,p2_2,p1_1)*cross(p2_1,p2_2,p1_2) <= 0):
						return True
	return False

				 
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
def find1stLayerCenter():
	result = [0,0]
	result[0] = LENGTH_OF_MAP / 2.0
	result[1] = LENGTH_OF_MAP / 2.0
	return result

# Find best 1st layer rotation degree
def find1stLayerRotDeg(m_I,theta,best1stCenter):
	return 0

# Find 1st layer districting points
def find1stLayerDistrictPoint():
	district_angle = 360.0 / m_I
   	##############NOT DONE YET############################################################
   	#best_rotate_degree = find1stLayerRotDeg(m_I,theta,best1stCenter)					 #
	######################################################################################
	center = find1stLayerCenter()
	end_points = [[float(center[0])+LENGTH_OF_MAP*math.sqrt(2),float(center[1])]]
	end_points[0] = Rotate(center,end_points[0], best_rotate_degree_1st)
	result = []
	for i in range(m_I-1):
		end_points.append(Rotate(center,end_points[i],district_angle))
	for i in range(0,len(end_points)):
		tmp_list = []
		result.append(tmp_list)
		delta_x = end_points[i][0] - center[0]
		delta_y = end_points[i][1] - center[1]
		slope = delta_y / delta_x
		rad = math.atan2(delta_y, delta_x) # radius
		deg = rad*180/math.pi # degree
		if(deg < 0):
			deg += 360
		# To count how many points we should draw on the line
		counter = int(math.sqrt(delta_x**2 + delta_y**2)) // INTERVAL # //: integer division
		result[i].append([center[0], center[1]])
		for j in range(counter): # counter: length of districting line / INTERVAL
			tmp_x = result[i][j-1][0] + INTERVAL*math.cos(rad)
			tmp_y = result[i][j-1][1] + INTERVAL*math.sin(rad)
			# if tmp_x or tmp_y is out of bound, ignore it and stop the for-loop
			#if(tmp_x < 0 or tmp_x > LENGTH_OF_MAP or tmp_y < 0 or tmp_y > LENGTH_OF_MAP):
			if(out_of_bound([tmp_x,tmp_y], center,1)):
				break
			result[i].append([tmp_x,tmp_y])
	return result

#Find 2st layer center
def find2ndLayerCenter():
	#### NOTICE: Must add points into set counter-clockwise/clockwise, otherwise might get incorrect centroid
	#### NOTICE: This method can only be apply to "Square Map"
	#### NOTICE: This function still has a lot to improve, if we want to apply this method on a more general map
	result = []
	points_set = []
	#### Find points of each polygon in map
	for i in range(m_I):
		p1 = district_points_1st[i][len(district_points_1st[i])-1]
		temp = []	
		points_set.append(temp)
		points_set[i].append(best1stCenter)
		points_set[i].append(p1)

	flg = [False, False, False, False]
	deg = [45,135,225,315]
	# cps: corner points
	cps = [[LENGTH_OF_MAP,LENGTH_OF_MAP],[0,LENGTH_OF_MAP],[0,0],[LENGTH_OF_MAP,0]]
	for i in range(m_I-1):
		deg1 = (360/m_I)*i + best_rotate_degree_1st 
		deg2 = (360/m_I)*(i+1) + best_rotate_degree_1st
		for j in range(4):
			if(flg[j] == False and (deg1 <= deg[j] and deg2 > deg[j])):
				flg[j] = True
				points_set[i].append(cps[j])
	# -1 index is to make sure we add points into set counter-clockwise
	for i in range(3,-1,-1):
		if(flg[i] == False):
			points_set[m_I-1].append(cps[i])
	for i in range(m_I):
		# add last point into set
		p2 = district_points_1st[(i+1)%m_I][len(district_points_1st[(i+1)%m_I])-1]
		points_set[i].append(p2)
		# temp variable representing [x,y]
		tmp_p = [-1,-1]
		# a Polygon in Library 'shapely'
		p = Polygon(points_set[i])
		# get centroid of polygon 'p'
		# cetroid is a POINT type in 'shapely'
		centroid = p.centroid
		tmp_p[0] = centroid.x
		tmp_p[1] = centroid.y
		result.append(tmp_p)
	return result
# find 2nd layer districting points
def find2ndLayerDistrictPoint():
	global best1stCenter, district_points_1st
	district_angle = 360.0 / (m_O+1)
	result = []
	for i in range(len(best2ndCenter)):
		# single layer districting points
		single_layer_dp = []

		# make single district endpoints in 2-layer design
		end_points = []
		end_points.append(Rotate(best2ndCenter[i], best1stCenter, district_angle / 2.0))
		for j in range(m_O):
			end_points.append(Rotate(best2ndCenter[i], end_points[j], district_angle))
		for j in range(len(end_points)):
			tmp_list = []
			delta_x = end_points[j][0] - best2ndCenter[i][0]
			delta_y = end_points[j][1] - best2ndCenter[i][1]
			rad = math.atan2(delta_y, delta_x) #radius
			counter = max(MAGIC_NUMBER,int(math.sqrt(delta_x**2 + delta_y**2)) // INTERVAL) # //: integer division
			tmp_list.append([best2ndCenter[i][0],best2ndCenter[i][1]])
			for k in range(counter):
				tmp_x = tmp_list[k][0] + INTERVAL*math.cos(rad)
				tmp_y = tmp_list[k][1] + INTERVAL*math.sin(rad)
				#if(tmp_x < 0 or tmp_x > LENGTH_OF_MAP or tmp_y < 0 or tmp_y > LENGTH_OF_MAP):
				if(out_of_bound([tmp_x,tmp_y], best2ndCenter[i],2) == True):
					break
				tmp_list.append([tmp_x,tmp_y])
			single_layer_dp.append(tmp_list)
		result.append(single_layer_dp)	
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
	global best1stCenter, best2ndCenter, theta, district_points_1st
	print("*******************  Show Map  ************************")
	
	
	# plot boundary points
	for point in dataBound[1:]:
		plt.plot(point[0], point[1], 'k.') # k.: black point
		
	# plot customer points	
	for point in dataCust[1:]:
		plt.plot(point[0], point[1], 'b.') # b.: blue point
	
	# plot 1st layer districting line
	
	for i in range(len(district_points_1st)):
		for j in range(len(district_points_1st[i])):
			plt.plot(district_points_1st[i][j][0], district_points_1st[i][j][1], 'g.')
	
	# plot best 1st layer center
	plt.plot(best1stCenter[0], best1stCenter[1], 'r.') # r.: red point
	
	################### Plot two layer #######################
	if(w == 2):
		# plot 2nd layer districting line
		for i in range(len(district_points_2nd)):
			for j in range(len(district_points_2nd[i])):
				for k in range(1,len(district_points_2nd[i][j])):
					plt.plot(district_points_2nd[i][j][k][0],district_points_2nd[i][j][k][1], 'y.')
		
		# plot best 2nd layer center
		for c in best2ndCenter:
			plt.plot(c[0],c[1], 'm.')
	
	# show graph
	plt.xticks(np.arange(0,LENGTH_OF_MAP+1,LENGTH_OF_MAP/10))
	plt.yticks(np.arange(0,LENGTH_OF_MAP+1,LENGTH_OF_MAP/10))
	plt.axis('equal')
	plt.show()


##################################### Main Function #################################
Read_file()
show_info()
# find districting points in 1st layer
district_points_1st = find1stLayerDistrictPoint()
# find center of 1st layer
best1stCenter = find1stLayerCenter()
if(w == 2):
	# find center of each sub-district in 2nd layer design
	best2ndCenter = find2ndLayerCenter()
	# find districting points of each sub-district in 2nd layer design
	district_points_2nd = find2ndLayerDistrictPoint()
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
	