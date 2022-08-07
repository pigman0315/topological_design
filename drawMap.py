import numpy as np
import matplotlib.pyplot as plt
import math
import random
import sys
import copy
import os
from shapely.geometry import Polygon
from shapely import wkt
import csv
############################ Variable #################################
# Constant
SINGLE_ROT_DEG = 15
EPSILON = 0.1
TIME_PERIOD_NUM = 6
PEAK_CUSTOMER_THRESHOLD = 15 # when the number of customers in the time period over the threshold, it is a  peak time period
CUSTOMER_RATIO = [1/25,5/25,10/25,4/25,2/25,3/25] # customer ratio in each time period of a district (total time period = 6 (might be 12) for testing)
SPEED = 200 # unit: (m/hr)
maxN = 3 # max number of candidate exchange points in a circle
minN = 2 # min number of candidate exchange points in a circle
H = 2.0
SERV_COST = 0.0167
VISUALIZE = False

# Parameter
# m_I = 4 --> degree = 90, m_I = 3 ---> degree = 120
# m_O = 3 --> degree = 90, m_O = 2 ---> degree = 120
# w = 1 ---> single layer, w = 2 ---> two layer
max_x = -1
min_x = -1
max_y = -1
min_y = -1
interval = -1
max_side_length = -1
m_I = -1
m_O = -1
w = -1
best_rot_deg = -1
T = -1
r = -1# be used to create candidate exchange point around centroid of each sub-district, unit: meter
a = -1 # be used to calculate weight of each candidate exchange point, unit: meter
PEAK_TIME_LIST = []

# Data points from files
dataBound = None
dataCent = None
dataCust = None

# Distringing end points
district_end_points_1st = None
district_end_points_2nd = None

# Districting points
district_points_1st = None
district_points_2nd = None

# center
best1stCenter = None
best2ndCenter = []

# end points of districting line
district_end_points_1st = None
# districted custormer points
distr_cust_points_1st = None
# candiate exchange points
cand_exch_point = None
# weight of candiate exchange points
cep_weight = None
# exchange point layer
exch_point_1st = None
exch_point_2nd = None
########################### Function Definition #####################
def tmp(cand_exch_point,dcp):
	cp = dcp[2]
	cep = cand_exch_point[2]
	for i in range(6,10):
		for j in range(6,10):
			time = math.sqrt((cp[i][0]-cp[j][0])**2 + (cp[i][1]-cp[j][1])**2)/40000
			time *= 60;
			print('%.2f' % time,sep=' ')

def show_info():
	print("*******************  Map Information  *****************")
	print("max length x of the map: ", max_x,"(m)")
	print("max length y of the map: ", max_y,"(m)")
	print("min length x of the map: ", min_x,"(m)")
	print("min length y of the map: ", min_y,"(m)")
	print("Interval between points: ", interval,"(m)")
	print("Best 1st layer rotation degree: ", best_rotate_degree_1st , "(degree)")
	print("1-layer or 2-layer:", w)
	print("Number of inner district(m_I): ", m_I)
	print("Number of outter district(m_O): ", m_O)
	print("Guarantee service time H: ", H, "(hr)")
	print("Guarantee service time H: ", H*60, "(min)")
	print("Travel time T: ", T, "(hr)")
	print("Travel time T: ", T*60,"(min)")
	print("Speed of courier: ",SPEED, "(km/hr)")
	print("Speed of courier: ",SPEED*1000//60, "(m/min)")
# subfunction for out_of_bound to return the cross value of p1p2 line & p1p3 line 
def cross(p1,p2,p3):
	x1=p2[0]-p1[0]
	y1=p2[1]-p1[1]
	x2=p3[0]-p1[0]
	y2=p3[1]-p1[1]
	return x1*y2-x2*y1 
# check if the points out of bound
def out_of_bound(point, center, mode, I):
	district_angle = 360.0 / I
	if(point[0] < min_x or point[0] > max_x or point[1] < min_y or point[1] > max_y):
		return True
	if(mode == 2):
			# get endpoint of 1st layer districting lines
			end_points = []
			for i in range(I):
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
	result[0] = (max_x+min_x) / 2.0
	result[1] = (max_y+min_y) / 2.0
	return result

# Find best 1st layer rotation degree
def find1stLayerRotDeg(I,theta,best1stCenter):
	return 0
def find1stDistrictEndPoint(I,district_points_1st, best1stCenter):
	cps_deg = [45,135,225,315]
	cps = [[max_x,max_y],[min_x,max_y],[min_x,min_y],[max_x,min_y]]
	cps_used = [False]*4
	result = []
	c = best1stCenter
	for i in range(I):
		tmp = []
		ep1 = district_points_1st[i][len(district_points_1st[i])-1]
		ep2 = district_points_1st[(i+1)%I][len(district_points_1st[(i+1)%I])-1]
		dx1 = ep1[0] - c[0]
		dy1 = ep1[1] - c[1]
		dx2 = ep2[0] - c[0]
		dy2 = ep2[1] - c[1]
		rad1 = math.atan2(dy1, dx1) # radius
		deg1= rad1*180/math.pi # degree
		if(deg1 < 0):
			deg1 += 360
		rad2 = math.atan2(dy2, dx2) # radius
		deg2= rad2*180/math.pi # degree
		if(deg2 < 0):
			deg2 += 360
		if(deg2 < deg1):
			deg2 += 360
		# add center, 1st end point to tmp
		tmp.append(c)
		tmp.append(ep1)
		# add corner points to tmp
		for i in range(4):
			if((cps_deg[i] >= deg1 and cps_deg[i] <= deg2)):
				tmp.append(cps[i])
				cps_used[i] = True
		for i in range(4):
			if((cps_deg[i]+360) >= deg1 and (cps_deg[i]+360) <= deg2 and cps_used[i] == False):
				tmp.append(cps[i])
				cps_used[i] = True
		# add 2nd end point to tmp
		tmp.append(ep2)
		result.append(tmp)
	return result

# Find 1st layer districting points
def find1stLayerDistrictPoint(I,rot_deg):
	district_angle = 360.0 / I
   	##############NOT DONE YET############################################################
   	#best_rotate_degree = find1stLayerRotDeg(m_I,theta,best1stCenter)					 #
	######################################################################################
	center = find1stLayerCenter()
	end_points = [[float(center[0])+max_side_length*math.sqrt(2),float(center[1])]]
	end_points[0] = Rotate(center,end_points[0], rot_deg)
	result = []
	for i in range(I-1):
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
		# Use to count how many points we should draw at most on the line
		counter = int(max_side_length*2 // interval) # //: integer division
		result[i].append([center[0], center[1]])
		for j in range(counter): # counter: length of districting line / interval
			tmp_x = result[i][j-1][0] + interval*math.cos(rad)
			tmp_y = result[i][j-1][1] + interval*math.sin(rad)
			# if tmp_x or tmp_y is out of bound, ignore it and stop the for-loop
			
			if(out_of_bound([tmp_x,tmp_y], center,1,I)):
				break
			result[i].append([tmp_x,tmp_y])
	return result

def isIntersectSegment(p,s,e,center):
	p1_1 = p
	p1_2 = center
	p2_1 = s
	p2_2 = e
	# step1: exclude lines whose max x/y is smaller than the other line's min x/y
	if(max(p1_1[0],p1_2[0]) > min(p2_1[0],p2_2[0])
	and max(p2_1[0],p2_2[0]) > min(p1_1[0],p1_2[0])
	and max(p1_1[1],p1_2[1]) > min(p2_1[1],p2_2[1])
	and max(p2_1[1],p2_2[1]) > min(p1_1[1],p1_2[1])):
		# cross product checking
		if(cross(p1_1,p1_2,p2_1)*cross(p1_1,p1_2,p2_2) < 0
			and cross(p2_1,p2_2,p1_1)*cross(p2_1,p2_2,p1_2) < 0):
			return True
	return False
def in_poly(p,poly,center): 
	sinsc = 0
	for i in range(len(poly)):
		s = poly[i]
		e = poly[(i+1)%len(poly)]
		if(isIntersectSegment(p,s,e,center)):
			return False
	return True;
# districting 1 layer customer point
def find1stLayerCust(I,dataCust, district_end_points_1st,centers):
	result = []
	for i in range(I):
		tmp = []
		result.append(tmp)
	for p in dataCust:
		for i in range(I):
			if(in_poly(p, district_end_points_1st[i],centers[i])):
				result[i].append(p)
				break
	return result
# get max travel time
def get_max_dist(w,I,O,distr_end_points,bestCenter):
	max_dist = 0.0
	if(w == 1):
		for i in range(I):
			c = bestCenter[i]
			for cp in distr_end_points[i]:
				dist = math.sqrt((cp[0] - c[0])**2 + (cp[1] - c[1])**2)
				if(dist > max_dist):
					max_dist = dist
	else:
		for i in range(I):
			for o in range(O+1):
				c = bestCenter[i][o]
				for cp in distr_end_points[i][o]:
					dist = math.sqrt((cp[0]-c[0])**2 + (cp[1]-c[1])**2)
					if(dist > max_dist):
						max_dist = dist
	return max_dist

#Find 2st layer center
def find2ndLayerCenter(I,district_end_points_1st):
	#### ATTENTION: Must add points into set counter-clockwise/clockwise, otherwise, you will get incorrect centroid
	#### ATTENTION: This method can only be apply to "Square Map"
	#### ATTENTION: This function still has a lot to improve, if we want to apply this method on a more general map
	result = []
	for i in range(I):
		# temp variable representing [x,y]
		tmp_p = [-1,-1]
		# a Polygon in Library 'shapely'
		p = Polygon(district_end_points_1st[i])
		# get centroid of polygon 'p'
		# cetroid is a POINT type in 'shapely'
		centroid = p.centroid
		tmp_p[0] = centroid.x
		tmp_p[1] = centroid.y
		result.append(tmp_p)
	return result
# find 2nd layer districting points
def find2ndLayerDistrictPoint(I,O):
	district_angle = 360.0 / (O+1)
	result = []
	for i in range(I):
		# single layer districting points
		single_layer_dp = []

		# make single district endpoints in 2-layer design
		end_points = []
		end_points.append(Rotate(best2ndCenter[i], best1stCenter, district_angle / 2.0))
		for j in range(O+1):
			end_points.append(Rotate(best2ndCenter[i], end_points[j], district_angle))
		for j in range(len(end_points)):
			tmp_list = []
			delta_x = end_points[j][0] - best2ndCenter[i][0]
			delta_y = end_points[j][1] - best2ndCenter[i][1]
			rad = math.atan2(delta_y, delta_x) #radius
			# Use to count how many points we should draw at most on the line
			counter = max_side_length*2 // interval # //: integer division
			tmp_list.append([best2ndCenter[i][0],best2ndCenter[i][1]])
			for k in range(counter):
				tmp_x = tmp_list[k][0] + interval*math.cos(rad)
				tmp_y = tmp_list[k][1] + interval*math.sin(rad)
				if(out_of_bound([tmp_x,tmp_y], best2ndCenter[i],2,I) == True):
					break
				tmp_list.append([tmp_x,tmp_y])
			single_layer_dp.append(tmp_list)
		result.append(single_layer_dp)	
	return result
def find2ndDistrictEndPoint(district_end_points_1st, best2ndCenter, district_points_2nd,I,O):
	result = []
	for i in range(I):
		tmp1 = []
		bps = district_end_points_1st[i]
		c = best2ndCenter[i]
		bps_deg = []
		for p in bps:
			dx = p[0] - c[0]
			dy = p[1] - c[1]
			rad = math.atan2(dy, dx) # radius
			deg= rad*180/math.pi # degree
			if(deg < 0):
				deg += 360
			bps_deg.append(deg)
		for j in range(O+1):
			tmp2 = []
			# calculate 2nd districting line degree with respect to center of 2nd layer
			ep1 = district_points_2nd[i][j][len(district_points_2nd[i][j])-1]
			ep2 = district_points_2nd[i][(j+1)%(O+1)][len(district_points_2nd[i][(j+1)%(O+1)])-1]
			dx1 = ep1[0] - c[0]
			dy1 = ep1[1] - c[1]
			dx2 = ep2[0] - c[0]
			dy2 = ep2[1] - c[1]
			rad1 = math.atan2(dy1, dx1) # radius
			deg1= rad1*180/math.pi # degree
			if(deg1 < 0):
				deg1 += 360
			rad2 = math.atan2(dy2, dx2) # radius
			deg2= rad2*180/math.pi # degree
			if(deg2 < 0):
				deg2 += 360
			if(deg2 < deg1):
				deg2 += 360
			# add center, 1st end point to tmp2
			tmp2.append(c)
			tmp2.append(ep1)
			# add 1st-layer end point to tmp2
			for idx in range(len(bps_deg)):
				if((bps_deg[idx] >= deg1 and bps_deg[idx] <= deg2) or ((bps_deg[idx]+360) >= deg1 and (bps_deg[idx]+360) <= deg2)):
					tmp2.append(bps[idx])
			# add 2nd end point
			tmp2.append(ep2)
			# append tmp2 to tmp1
			tmp1.append(tmp2)
		result.append(tmp1)
	return result
def find2ndLayerCust(dataCust, district_end_points, I, O,centers):
	result = []
	used = [False]*len(dataCust)
	for i in range(I):
		tmp1 = []
		for j in range(O+1):
			tmp2 = []
			for k in range(len(dataCust)):
				if(used[k] == False and in_poly(dataCust[k], district_end_points[i][j],centers[i][j])):
					tmp2.append(dataCust[k])
					used[k] = True
			tmp1.append(tmp2)
		result.append(tmp1)
	return result
def find3rdLayerCenter(district_end_points_2nd, I, O):
	result = []
	for i in range(I):
		tmp = []
		for j in range(O+1):
			ps = district_end_points_2nd[i][j]
			# temp variable representing [x,y]
			tmp_p = [-1,-1]
			# a Polygon in Library 'shapely'
			p = Polygon(ps)
			# get centroid of polygon 'p'
			# cetroid is a POINT type in 'shapely'
			centroid = p.centroid
			tmp_p[0] = centroid.x
			tmp_p[1] = centroid.y
			tmp.append(tmp_p)
		result.append(tmp)
	return result
# Find best rotation degree
def getDispersionValue(w,I,O,distr_cust_points,bestCenter):
	avg_dist = 0
	max_dist = 0
	omega = 0.5 # omega is a weighted parameter of avg & max
	delta = 0 # dispersion value of specific layer design
	if(w == 1):	
		for i in range(I):
			cps = distr_cust_points[i] # customer point set
			c = bestCenter[i] # center
			total_dist = 0
			for p in cps:
				dist = math.sqrt((p[0]-c[0])**2 + (p[1]-c[1])**2)
				total_dist += dist
			if(total_dist > max_dist):
				if(len(cps) != 0):
					max_dist = total_dist / len(cps)
			if(len(cps) != 0):
				avg_dist += total_dist / len(cps)
		avg_dist /= I
		delta = omega*avg_dist + (1-omega)*max_dist
		
	else:
		for i in range(I):
			for o in range(O+1):
				cps = distr_cust_points[i][o] # customer point set
				c = bestCenter[i][o] # center
				total_dist = 0
				for p in cps:
					dist = math.sqrt((p[0]-c[0])**2 + (p[1]-c[1])**2)
					total_dist += dist
				if(total_dist > max_dist):
					if(len(cps) != 0):
						max_dist = total_dist / len(cps)
				if(len(cps) != 0):
					avg_dist += total_dist / len(cps)
		avg_dist /= (I*(O+1))
		delta = omega*avg_dist + (1-omega)*max_dist
	return delta
# randomly create candidate exchange point
def getCandExchPoint(w,I,O,bestCenter,r,maxN,minN):
	# r is the radius of circle
	# maxN is the max number of candidate exchange point in each circle
	# minN is the min number of candidate exchange point in each circle
	random.seed(22) # for debug
	result = []
	if(w == 1):
		for i in range(I):
			tmp = []
			c = bestCenter[i]
			n = random.randint(minN,maxN)
			for j in range(n):
				node = [-1,-1]
				theta = random.uniform(0,2*math.pi)
				node[0] = int(r*math.cos(theta)	+ c[0])
				node[1] = int(r*math.sin(theta) + c[1])
				tmp.append(node)
			result.append(tmp)

	else:
		for i in range(I):
			tmp1 = []
			for o in range(O+1):
				tmp2 = []
				c = bestCenter[i][o]
				n = random.randint(minN,maxN)
				for j in range(n):
					node = [-1,-1]
					theta = random.uniform(0,2*math.pi)
					node[0] = r*math.cos(theta)	+ c[0]
					node[1] = r*math.sin(theta) + c[1]
					tmp2.append(node)
				tmp1.append(tmp2)
			result.append(tmp1)
	return result		
# get weight of candidate exchange point 
def getCepWeight(w,I,O,a,cand_exch_point,districted_customer_points):
	result = []
	if(w == 1):
		for i in range(I):
			# customer points
			cps = districted_customer_points[i]
			tmp = []
			for cep in cand_exch_point[i]:
				cnt = 0
				for p in cps:
					dist = math.sqrt((p[0]-cep[0])**2 + (p[1]-cep[1])**2)
					if(dist < a):
						cnt += 1
				# give a basis count 1, to avoid situation when there are no customer in the region
				tmp.append(cnt+1) 
			result.append(tmp)

	else:
		for i in range(I):
			tmp1 = []
			for o in range(O+1):
				# customer points
				cps = districted_customer_points[i][o]
				tmp2 = []
				for cep in cand_exch_point[i][o]:
					cnt = 0
					for p in cps:
						dist = math.sqrt((p[0]-cep[0])**2 + (p[1]-cep[1])**2)
						if(dist < a):
							cnt += 1
					tmp2.append(cnt+1)
				tmp1.append(tmp2)
			result.append(tmp1)
	return result
# make a graph of adjecency matrix
def makeGraph(w,m_I,m_O,cep_weight,cep,EPSILON):
	# cep: candiate exchange point
	# array to record each cand exch point's number
	num_ary = [0]*(len(cep))
	for i in range(1,len(cep)):
		num_ary[i] = num_ary[i-1]+len(cep[i-1])
	# array to record each cand exch point
	point_ary = []
	for i in range(len(cep)):
		for p in cep[i]:
			point_ary.append(p)
	
	# initialization
	cnt = len(point_ary)
	G = [-1]*cnt
	for i in range(cnt):
		G[i] = [-1]*cnt
	#g = 0
	for i in range(1,len(cep)):
		for j in range(len(cep[i])):
			for k in range(len(cep[i-1])):
				u = cep[i-1][k]
				v = cep[i][j]
				dist = math.sqrt((u[0]-v[0])**2 + (u[1]-v[1])**2)
				
				if((dist/SPEED) <= T + EPSILON):
					#g+=1
					G[num_ary[i-1]+k][num_ary[i]+j] = cep_weight[i][j]
	return G
def calculateInDeg(G):
	n = len(G)
	in_deg = [0]*n
	for j in range(n):
		cnt=0
		for i in range(n):
			if(G[i][j] != -1 and G[i][j] != 0):
				cnt+=1
		in_deg[j] = cnt
	return in_deg
# topological sort
def topoSort(graph):
	result = []
	# make a copy to prevent graph from being modified
	G = copy.deepcopy(graph)
	n = len(G)
	in_deg  = calculateInDeg(G)
	used = [False]*n
	while(sum(in_deg)>0):
		for i in range(n):
			if(used[i] == False and in_deg[i] == 0):
				used[i] = True
				for j in range(n):
					G[i][j] = -1
				in_deg  = calculateInDeg(G)
	
				result.append(i)
	return result
# relaxation function for DAG shortest path problem
def relax(G,u,v,shortest,pred):
	if(shortest[u] + G[u][v] < shortest[v]):
		pred[v] = u
		shortest[v] = shortest[u] + G[u][v]

# get path by shortest & predecessor 
def getPath(shortest,pred,cep):
	n = len(shortest)
	# map node number to node coordinate
	coord = []
	for i in range(len(cep)):
		for j in range(len(cep[i])):
			coord.append(cep[i][j])
	# find min value & min index
	idx = -1
	minV = sys.maxsize
	for i in range(n):
		if(minV > shortest[i]):
			minV = shortest[i]
			idx = i
	tmp = []
	tmp.append(idx)
	# get reverse path
	while(1):
		idx = pred[idx]
		if(pred[idx] == -1):
			break
		tmp.append(idx)
	# reverse it & add source
	path = []
	for i in range(len(tmp)-1,-1,-1):
		path.append(coord[tmp[i]])
	path.append(coord[tmp[len(tmp)-1]])
	return path

# get exchange point
def getExchPoint(w,I,O,cep_weight,cand_exch_point,EPSILON):
	# l = 1 --> inner layer, l = 2 --> outer layer
	result = []
	if(w == 1):
		if(I == 3):
			min_dist = sys.float_info.max
			for p0 in cand_exch_point[0]:
				for p1 in cand_exch_point[1]:
					for p2 in cand_exch_point[2]:
						dist = dist = math.sqrt((p0[0]-p1[0])**2 + (p0[1]-p1[1])**2) + math.sqrt((p1[0]-p2[0])**2 + (p1[1]-p2[1])**2) + math.sqrt((p2[0]-p0[0])**2 + (p2[1]-p0[1])**2)
						if(dist < min_dist):
							min_dist = dist
							result = [p0,p1,p2]
		elif(I == 4):
			min_dist = sys.float_info.max
			for p0 in cand_exch_point[0]:
				for p1 in cand_exch_point[1]:
					for p2 in cand_exch_point[2]:
						for p3 in cand_exch_point[3]:
							dist = dist = math.sqrt((p0[0]-p1[0])**2 + (p0[1]-p1[1])**2) + math.sqrt((p1[0]-p2[0])**2 + (p1[1]-p2[1])**2) + math.sqrt((p2[0]-p3[0])**2 + (p2[1]-p3[1])**2) + math.sqrt((p3[0]-p0[0])**2 + (p3[1]-p0[1])**2)
							if(dist < min_dist):
								min_dist = dist
								result = [p0,p1,p2,p3]
		else:
			print("ERROR: incorrect m_I")
	else: # w = 2
		# First, calculate outer layer's longest path
		for i in range(I):
			if(O+1 == 3):
				min_dist = sys.float_info.max
				for p0 in cand_exch_point[0]:
					for p1 in cand_exch_point[1]:
						for p2 in cand_exch_point[2]:
							dist = dist = math.sqrt((p0[0]-p1[0])**2 + (p0[1]-p1[1])**2) + math.sqrt((p1[0]-p2[0])**2 + (p1[1]-p2[1])**2) + math.sqrt((p2[0]-p0[0])**2 + (p2[1]-p0[1])**2)
							if(dist < min_dist):
								min_dist = dist
								result.append([p0,p1,p2])
			elif(O+1 == 4):
				min_dist = sys.float_info.max
				for p0 in cand_exch_point[0]:
					for p1 in cand_exch_point[1]:
						for p2 in cand_exch_point[2]:
							for p3 in cand_exch_point[3]:
								dist = dist = math.sqrt((p0[0]-p1[0])**2 + (p0[1]-p1[1])**2) + math.sqrt((p1[0]-p2[0])**2 + (p1[1]-p2[1])**2) + math.sqrt((p2[0]-p3[0])**2 + (p2[1]-p3[1])**2) + math.sqrt((p3[0]-p0[0])**2 + (p3[1]-p0[1])**2)
								if(dist < min_dist):
									min_dist = dist
									result.append([p0,p1,p2,p3])
	return result
def get_best_layer_design():
	global district_points_1st, best1stCenter, district_end_points_1st, best2ndCenter
	global district_points_2nd, district_end_points_2nd, district_customer_points_2nd, best3rdCenter
	w = 1
	I = -1
	O = -1
	for i in range(3,5):
		min_dp = sys.maxsize;
		brg = -1.0;
		for rot_deg in range(0,360//i, SINGLE_ROT_DEG):
			print("Try m_I =",i,"& rotate degree =",rot_deg,"...",end=", ")
			# find districting points in 1-layer
			district_points_1st = find1stLayerDistrictPoint(i,rot_deg)
			# find center of 1st layer
			best1stCenter = find1stLayerCenter()
			# find end points of districting line
			district_end_points_1st = find1stDistrictEndPoint(i,district_points_1st, best1stCenter)
			# find center of each sub-district in 2-layer design
			best2ndCenter = find2ndLayerCenter(i,district_end_points_1st)
			# distribute customer points to its corresponding district
			district_customer_points_1st = find1stLayerCust(i,dataCust, district_end_points_1st,best2ndCenter)
			
			# check if single layer is ok
			max_d = get_max_dist(w,i,0,district_end_points_1st,best2ndCenter)
			T = H / (0.5+i)
			print("Max T = %.5f" % (max_d/SPEED), ", must under %.5f" % (T/2))
			if(max_d/SPEED <= 0.5*T):
				I = i
				dp = getDispersionValue(1,I,O,district_customer_points_1st,best2ndCenter)
				if(dp < min_dp):
					min_dp = dp
					brg = rot_deg
		if(brg != -1.0):
			return w,I,O,brg
		# if(max_d/SPEED > 0.5*T):
		# 	print("*** m_I =",i,"cannot guarantee the service time *** ")
	# Try 2-layer design
	w = 2
	for i in range(3,5):
		for o in range(2,4):
			min_dp = sys.maxsize;
			brg = -1.0;
			for rot_deg in range(0,360//i, SINGLE_ROT_DEG):
				print("Try m_I = {}, m_O = {}, deg = {}".format(i,o,rot_deg),end=", ")	
				# find districting points in 1-layer
				district_points_1st = find1stLayerDistrictPoint(i,rot_deg)
				# find center of 1st layer
				best1stCenter = find1stLayerCenter()
				# find end points of districting line
				district_end_points_1st = find1stDistrictEndPoint(i,district_points_1st, best1stCenter)
				# distribute customer points to its corresponding district
				district_customer_points_1st = find1stLayerCust(i,dataCust, district_end_points_1st,best2ndCenter)
				# find center of each sub-district in 2-layer design
				best2ndCenter = find2ndLayerCenter(i,district_end_points_1st)
				# find districting points of each sub-district in 2nd layer design
				district_points_2nd = find2ndLayerDistrictPoint(i,o)
				# find end points of each sub-sub-district
				district_end_points_2nd = find2ndDistrictEndPoint(district_end_points_1st, best2ndCenter, district_points_2nd, i, o)
				# find center of each sub-sub-district
				best3rdCenter = find3rdLayerCenter(district_end_points_2nd, i, o)
				# distribute customer points to its corresponding district
				district_customer_points_2nd = find2ndLayerCust(dataCust, district_end_points_2nd,i,o,best3rdCenter)
				
				# get max distance of client
				max_d = get_max_dist(w,i,o,district_end_points_2nd,best3rdCenter)
				T = H / (0.5+i+2*(w-1)*o)
				print("Max T = %.2f" % (max_d/SPEED), ", must under %.2f" % (T/2))
				if(max_d/SPEED <= 0.5*T):
					I = i
					O = o
					dp = getDispersionValue(2,I,O,district_customer_points_2nd,best3rdCenter)
					if(dp < min_dp):
						min_dp = dp
						brg = rot_deg
			if(brg != -1.0):
				return w,I,O,brg
			# if(max_d/SPEED > 0.5*T):
			# 	print("*** m_I =",i,"m_O = ",o,"cannot guarantee the service time ***")
# Read data points from files
def read_file():
	global dataBound,dataCent,dataCust,min_x,min_y,max_x,max_y,interval,max_side_length
	# open file (file path may use '/' or '\\')
	curPath = os.path.dirname(os.path.abspath(__file__))
	fileBound = open(curPath+"/boundaries.txt", "r")
	fileCust = open(curPath+"/customers.txt", "r")

	# read side_length_info/boundaries/centers/customers data points from file
	side_length_info = fileBound.readline().split(' ')
	dataBound = fileBound.readlines()
	fileBound.close()
	dataCust = fileCust.readlines()
	fileCust.close()

	# convert (x,y) from string to float, e.g.: ('10','10') ---> (10,10)
	min_x = float(side_length_info[0])
	max_x = float(side_length_info[1])
	min_y = float(side_length_info[2])
	max_y = float(side_length_info[3])
	interval = float(side_length_info[4])
	max_side_length = max(max_x,max_y)
	for i in range(0,len(dataBound)):
		dataBound[i] = dataBound[i].split()
		dataBound[i][0] = float(dataBound[i][0])
		dataBound[i][1] = float(dataBound[i][1])
	for i in range(0,len(dataCust)):
		dataCust[i] = dataCust[i].split()
		dataCust[i][0] = float(dataCust[i][0])
		dataCust[i][1] = float(dataCust[i][1])

# Draw map for visualization
def draw_map():
	global best1stCenter, best2ndCenter, best3rdCenter, theta, district_points_1st
	
	
	# plot boundary points
	for point in dataBound[1:]:
		plt.plot(point[0], point[1], 'k.') # k.: black point
		
	# plot customer points	
	for i in range(len(districted_customer_points_1st)):
		for p in districted_customer_points_1st[i]:
			plt.plot(p[0], p[1], 'b.')
	
	
	# plot 1st layer districting line
	for i in range(len(district_points_1st)):
		for j in range(len(district_points_1st[i])):
			plt.plot(district_points_1st[i][j][0], district_points_1st[i][j][1], 'g.')
	
	# plot best 1st layer center
	plt.plot(best1stCenter[0], best1stCenter[1], 'r.') # r.: red point
	
	# plot best 2nd layer center
	for c in best2ndCenter:
		plt.plot(c[0],c[1], 'm.')
	
	if(w == 1):
		# draw candidate exchange point
		for i in range(m_I):
			for p in cand_exch_point[i]:
				plt.plot(p[0],p[1],color='orange',marker='.')
		# draw ring network
		for i in range(m_I):
			x = exch_point_1st[i][0]
			y = exch_point_1st[i][1]
			dx = exch_point_1st[(i+1)%m_I][0] - x
			dy = exch_point_1st[(i+1)%m_I][1] - y
			plt.arrow(x,y,dx,dy,width=max_x*0.005,length_includes_head = True,color='r')
	################### Plot two layer #######################
	if(w == 2):
		# plot 2nd layer districting line
		for i in range(len(district_points_2nd)):
			for j in range(len(district_points_2nd[i])):
				for k in range(1,len(district_points_2nd[i][j])):
					plt.plot(district_points_2nd[i][j][k][0],district_points_2nd[i][j][k][1], 'y.')
		
		# plot best 3rd layer center
		for i in range(m_I):
			for j in range(m_O+1):
				plt.plot(best3rdCenter[i][j][0],best3rdCenter[i][j][1],'co')
		# draw candidate exchange point
		for i in range(m_I):
			for j in range(m_O+1):
				for p in cand_exch_point[i][j]:
					plt.plot(p[0],p[1],color='orange',marker='.')
		# draw ring network
		for i in range(m_I):
			for o in range(m_O+1):
				x = exch_point_2nd[i][o][0]
				y = exch_point_2nd[i][o][1]
				dx = exch_point_2nd[i][o+1][0] - x
				dy = exch_point_2nd[i][o+1][1] - y
				plt.arrow(x,y,dx,dy,max_x*0.005,length_includes_head = True)
		for i in range(m_I):
			x = exch_point_1st[i][0]
			y = exch_point_1st[i][1]
			dx = exch_point_1st[(i+1)%m_I][0] - x
			dy = exch_point_1st[(i+1)%m_I][1] - y
			plt.arrow(x,y,dx,dy,max_x*0.005,length_includes_head = True,color='r')
	# show graph
	plt.xticks(np.arange(min_x,max_x+1,interval))
	plt.yticks(np.arange(min_y,max_y+1,interval))
	plt.axis('equal')
	plt.show()
def output_testcase_1(customer_points,cand_exch_point,center):
	I = m_I
	distr_num = len(customer_points)
	# Table1: #Customer/district/time period
	csvfile = open('c_m_l.txt','w')
	writer = csv.writer(csvfile,delimiter=' ')
	t1 = np.array([])
	for i in range(distr_num):
		num = len(customer_points[i])
		row = []
		tmp_total = 0
		for j in range(TIME_PERIOD_NUM-1):
			row.append(round(num*CUSTOMER_RATIO[j]))
			tmp_total += round(num*CUSTOMER_RATIO[j])
		row.append(max(0,num-tmp_total))
		writer.writerow(row)
		t1 = np.append(t1,row,axis=0)
	t1 = t1.reshape(I,TIME_PERIOD_NUM)
	csvfile.close()
	# Table2: postal number
	csvfile = open('c_w.txt','w')
	writer = csv.writer(csvfile,delimiter=' ')
	postal_num = []
	for i in range(distr_num):
		num = len(customer_points[i])
		row = []
		c = center[i]

		for j in range(num):
			if(customer_points[i][j][1] >= c[1]):
				row.append(i*2)
			else:
				row.append(i*2+1)
		writer.writerow(row)
	csvfile.close()
	# Table4: exchange points' time distance
	np.set_printoptions(suppress=True)
	total_cep = 0
	ceps_list = []
	for i in range(len(cand_exch_point)):
		total_cep += len(cand_exch_point[i])
		for j in range(len(cand_exch_point[i])):
			ceps_list.append(cand_exch_point[i][j])
	ceps_list = np.array(ceps_list)
	ceps_list = ceps_list.reshape(total_cep,2)
	t4 = np.zeros((total_cep,total_cep))
	t4 += 9999
	for i in range(total_cep):
		for j in range(total_cep):
			if(i != j):
				dist = math.sqrt((ceps_list[i][0]-ceps_list[j][0])**2 + (ceps_list[i][1]-ceps_list[j][1])**2)
				time = (dist)/SPEED * 60
				t4[i][j] = time
	cnt = 0
	for i in range(len(cand_exch_point)):
		ceps = cand_exch_point[i]
		tmp = np.zeros((len(ceps),len(ceps)))
		tmp += 9999
		t4[cnt:cnt+len(ceps),cnt:cnt+len(ceps)] = tmp
		cnt += len(ceps)
	
	#
	csvfile = open('de.txt','w')
	writer = csv.writer(csvfile,delimiter=' ')
	for i in range(total_cep):
		writer.writerow(t4[i])
	csvfile.close()

	# Table6: postal Exchange point & customer point distance of each time period
	total_cep = 0
	for i in range(len(cand_exch_point)):
		total_cep += len(cand_exch_point[i])
	tmp_9999 = np.zeros((total_cep,total_cep))
	tmp_9999 += 9999
	t4 = tmp_9999 # NOTICE: t4 initialized to all 9999 here
	points = cand_exch_point[0].copy()
	points.extend(customer_points[0])
	cus = []
	idx = [0]*TIME_PERIOD_NUM
	t1 = t1.T
	for i in range(TIME_PERIOD_NUM):
		tmp_cus = []
		for j in range(len(t1[i])):
			tmp_cus.append(customer_points[j][int(idx[j]):int(idx[j]+t1[i][j])])
			idx[j] += t1[i][j]
		cus.append(tmp_cus)
	sum_time = np.sum(np.array(t1),axis=1,dtype=int)
	t6_all = []
	csvfile = open('dij.txt','w')
	writer = csv.writer(csvfile,delimiter=' ')
	for i in range(TIME_PERIOD_NUM): # I am sure it is TIME_PERIOD_NUM = 5，不同區搞在一起
		t6 = np.zeros((sum_time[i]+len(t4),sum_time[i]+len(t4)))
		t6 += 9999
		t6[:len(t4),:len(t4)] = t4
		cnt = len(t4)
		for j in range(len(cus[i])):
			leng = len(cus[i][j])
			mat = np.zeros((leng,leng))
			mat += 9999
			for k in range(leng):
				for h in range(leng):
					if(k != h):
						mat[k][h] = (math.sqrt((cus[i][j][k][0] - cus[i][j][h][0])**2 + (cus[i][j][k][1] - cus[i][j][h][1])**2) / SPEED) * 60
			t6[cnt:cnt+leng,cnt:cnt+leng] = mat
			cnt += leng
		cnt_x = 0
		cnt_y = 0
		for j in range(I):
			mat = np.zeros((len(cand_exch_point[j]),len(cus[i][j])))
			for k in range(len(cand_exch_point[j])):
				for l in range(len(cus[i][j])):
					mat[k][l] = (math.sqrt((cand_exch_point[j][k][0]-cus[i][j][l][0])**2 + (cand_exch_point[j][k][1]-cus[i][j][l][1])**2) / SPEED) * 60
			t6[cnt_x:cnt_x+len(cand_exch_point[j]),len(t4)+cnt_y:len(t4)+cnt_y+len(cus[i][j])] = mat
			t6[len(t4)+cnt_y:len(t4)+cnt_y+len(cus[i][j]),cnt_x:cnt_x+len(cand_exch_point[j])] = mat.T
			cnt_x += len(cand_exch_point[j])
			cnt_y += len(cus[i][j])
		#writer.writerows(t6)
		writer.writerows(t6)
	csvfile.close()

	# Table7: postal number of each district
	csvfile = open('pw.txt','w')
	writer = csv.writer(csvfile,delimiter=' ')
	t7 = np.zeros((I,I*2),dtype=int)
	for i in range(I):
		t7[i][i*2] = 1
		t7[i][i*2+1] = 1
		writer.writerow(t7[i])
	csvfile.close()

	# Table8: peak time table
	csvfile = open('lu.txt','w')
	writer = csv.writer(csvfile,delimiter=' ')
	t8 = np.zeros((TIME_PERIOD_NUM,I),dtype=int)
	for t in range(TIME_PERIOD_NUM):
		if(np.sum(t1,axis=1)[t] >= PEAK_CUSTOMER_THRESHOLD):
			t8[t] = [str(1)]*I
	t8 = np.transpose(t8) # workaround: t8 (IxTIME_PERIOD) is actually the correct one
	writer.writerows(t8)
	csvfile.close()
	# 
def output_testcase_2(customer_points_list,cand_exch_point_list,center_list):
	I = m_O+1
	for t in range(m_I):
		customer_points = customer_points_list[t]
		cand_exch_point = cand_exch_point_list[t]
		center = center_list[t]
		dir_path = str(t)+'/'
		distr_num = len(customer_points)
		# Table1: #Customer/district/time period
		csvfile = open(dir_path+'c_m_l.txt','w')
		writer = csv.writer(csvfile,delimiter=' ')
		t1 = np.array([])
		for i in range(distr_num):
			num = len(customer_points[i])
			row = []
			for j in range(TIME_PERIOD_NUM-1):
				row.append(num//TIME_PERIOD_NUM)
			row.append(num-(num//TIME_PERIOD_NUM)*(TIME_PERIOD_NUM-1))
			writer.writerow(row)
			t1 = np.append(t1,row,axis=0)
		t1 = t1.reshape(I,TIME_PERIOD_NUM)
		csvfile.close()
		# Table2: postal number
		csvfile = open(dir_path+'c_w.txt','w')
		writer = csv.writer(csvfile,delimiter=' ')
		postal_num = []
		for i in range(distr_num):
			num = len(customer_points[i])
			row = []
			c = center[i]

			for j in range(num):
				if(customer_points[i][j][1] >= c[1]):
					row.append(i*2)
				else:
					row.append(i*2+1)
			writer.writerow(row)
		csvfile.close()
		# Table4: exchange points' time distance
		np.set_printoptions(suppress=True)
		total_cep = 0
		ceps_list = []
		for i in range(len(cand_exch_point)):
			total_cep += len(cand_exch_point[i])
			for j in range(len(cand_exch_point[i])):
				ceps_list.append(cand_exch_point[i][j])
		ceps_list = np.array(ceps_list)
		ceps_list = ceps_list.reshape(total_cep,2)
		t4 = np.zeros((total_cep,total_cep))
		t4 += 9999
		for i in range(total_cep):
			for j in range(total_cep):
				if(i != j):
					dist = math.sqrt((ceps_list[i][0]-ceps_list[j][0])**2 + (ceps_list[i][1]-ceps_list[j][1])**2)
					time = (dist)/SPEED * 60
					t4[i][j] = time
		cnt = 0
		for i in range(len(cand_exch_point)):
			ceps = cand_exch_point[i]
			tmp = np.zeros((len(ceps),len(ceps)))
			tmp += 9999
			t4[cnt:cnt+len(ceps),cnt:cnt+len(ceps)] = tmp
			cnt += len(ceps)
		#
		csvfile = open(dir_path+'de.txt','w')
		writer = csv.writer(csvfile,delimiter=' ')
		for i in range(total_cep):
			writer.writerow(t4[i])
		csvfile.close()
		# Table6: postal Exchange point & customer point distance of each time period
		total_cep = 0
		for i in range(len(cand_exch_point)):
			total_cep += len(cand_exch_point[i])
		tmp_9999 = np.zeros((total_cep,total_cep))
		tmp_9999 += 9999
		t4 = tmp_9999 # NOTICE: t4 change to all 9999 here
		points = cand_exch_point[0].copy()
		points.extend(customer_points[0])
		cus = []
		idx = [0]*TIME_PERIOD_NUM
		t1 = t1.T
		for i in range(TIME_PERIOD_NUM):
			tmp_cus = []
			for j in range(len(t1[i])):
				tmp_cus.append(customer_points[j][int(idx[j]):int(idx[j]+t1[i][j])])
				idx[j] += t1[i][j]
			cus.append(tmp_cus)
		sum_time = np.sum(np.array(t1),axis=1,dtype=int)
		t6_all = []
		csvfile = open(dir_path+'dij.txt','w')
		writer = csv.writer(csvfile,delimiter=' ')
		for i in range(TIME_PERIOD_NUM): # I am sure it is TIME_PERIOD_NUM = 5，不同區搞在一起
			t6 = np.zeros((sum_time[i]+len(t4),sum_time[i]+len(t4)))
			t6 += 9999
			t6[:len(t4),:len(t4)] = t4
			cnt = len(t4)
			for j in range(len(cus[i])):
				leng = len(cus[i][j])
				mat = np.zeros((leng,leng))
				mat += 9999
				for k in range(leng):
					for h in range(leng):
						if(k != h):
							mat[k][h] = (math.sqrt((cus[i][j][k][0] - cus[i][j][h][0])**2 + (cus[i][j][k][1] - cus[i][j][h][1])**2) / SPEED) * 60
				t6[cnt:cnt+leng,cnt:cnt+leng] = mat
				cnt += leng
			cnt_x = 0
			cnt_y = 0
			for j in range(I):
				mat = np.zeros((len(cand_exch_point[j]),len(cus[i][j])))
				for k in range(len(cand_exch_point[j])):
					for l in range(len(cus[i][j])):
						mat[k][l] = (math.sqrt((cand_exch_point[j][k][0]-cus[i][j][l][0])**2 + (cand_exch_point[j][k][1]-cus[i][j][l][1])**2) / SPEED) * 60
				t6[cnt_x:cnt_x+len(cand_exch_point[j]),len(t4)+cnt_y:len(t4)+cnt_y+len(cus[i][j])] = mat
				t6[len(t4)+cnt_y:len(t4)+cnt_y+len(cus[i][j]),cnt_x:cnt_x+len(cand_exch_point[j])] = mat.T
				cnt_x += len(cand_exch_point[j])
				cnt_y += len(cus[i][j])
			#writer.writerows(t6)
			writer.writerows(t6)
		csvfile.close()

		# Table7: postal number of each district
		csvfile = open(dir_path+'pw.txt','w')
		writer = csv.writer(csvfile,delimiter=' ')
		t7 = np.zeros((I,I*2),dtype=int)
		for i in range(I):
			t7[i][i*2] = 1
			t7[i][i*2+1] = 1
			writer.writerow(t7[i])
		csvfile.close()

		# Table8: peak time table
		csvfile = open(dir_path+'lu.txt','w')
		writer = csv.writer(csvfile,delimiter=' ')
		t8 = np.zeros((TIME_PERIOD_NUM,I),dtype=int)
		for i in range(I):
			t8[PEAK_TIME_LIST[i]][i] = 1
		writer.writerows(t8)
		csvfile.close()
def output_info_1(w,m_I,m_O,best_rot_deg,best1stCenter,best2ndCenter,district_end_points_1st,districted_customer_points_1st,exch_point_1st):
	f = open('sp1_result.txt',mode='w')
	f.write(str(w)+'\n')
	f.write(str(m_I)+'\n')
	f.write(str(m_O)+'\n')
	f.write(str(best_rot_deg)+'\n')
	f.write(str(int(best1stCenter[0]))+' '+str(int(best1stCenter[1]))+'\n')
	for i in range(m_I):
		f.write(str(int(best2ndCenter[i][0]))+' '+str(int(best2ndCenter[i][1]))+'\n')
	for i in range(m_I):
		f.write(str(len(district_end_points_1st[i]))+"\n")
		for j in range(len(district_end_points_1st[i])):
			f.write(str(int(district_end_points_1st[i][j][0]))+' '+str(int(district_end_points_1st[i][j][1]))+'\n')
	for i in range(m_I):
		f.write(str(len(districted_customer_points_1st[i]))+"\n")
		for j in range(len(districted_customer_points_1st[i])):
			f.write(str(int(districted_customer_points_1st[i][j][0]))+' '+str(int(districted_customer_points_1st[i][j][1]))+'\n')
	for i in range(m_I):
		f.write(str(int(exch_point_1st[i][0]))+' '+str(int(exch_point_1st[i][1]))+'\n')
	f.close()
def output_info_2(w,m_I,m_O,best_rot_deg,best1stCenter,best2ndCenter,best3rdCenter,district_end_points_1st,district_end_points_2nd,districted_customer_points_2nd,exch_point_1st,exch_point_2nd):
	for i in range(m_I):
		f = open(str(i)+'/'+'sp1_result.txt',mode='w')
		f.write(str(w)+'\n')
		f.write(str(m_I)+'\n')
		f.write(str(m_O)+'\n')
		f.write(str(best_rot_deg)+'\n')
	
		f.write(str(int(best2ndCenter[i][0]))+' '+str(int(best2ndCenter[i][1]))+'\n')
		for o in range(m_O+1):
			f.write(str(int(best3rdCenter[i][o][0]))+' '+str(int(best3rdCenter[i][o][1]))+'\n')
		for o in range(m_O+1):	
			f.write(str(len(district_end_points_2nd[i][o]))+"\n")
			for k in range(len(district_end_points_2nd[i][o])):
				f.write(str(int(district_end_points_2nd[i][o][k][0]))+' '+str(int(district_end_points_2nd[i][o][k][1]))+'\n')
		for o in range(m_O+1):
			f.write(str(len(districted_customer_points_2nd[i][o]))+"\n")
			for k in range(len(districted_customer_points_2nd[i][o])):
				f.write(str(int(districted_customer_points_2nd[i][o][k][0]))+' '+str(int(districted_customer_points_2nd[i][o][k][1]))+'\n')
		#f.write(str(int(exch_point_1st[i][0]))+' '+str(int(exch_point_1st[i][1]))+'\n')
		for o in range(m_O+1):
			f.write(str(int(exch_point_2nd[i][o][0]))+' '+str(int(exch_point_2nd[i][o][1]))+'\n')
		f.close()
def test_show_exch_point_cost_1st(exch_point_1st):
	dist = 0.0
	for i in range(m_I):
		dist += math.sqrt((exch_point_1st[i][0]-exch_point_1st[(i+1)%m_I][0])**2 + (exch_point_1st[i][1]-exch_point_1st[(i+1)%m_I][1])**2)
	cost_in_hr = dist/SPEED
	cost_in_min = cost_in_hr*60.0
	return cost_in_min

### Main Function ###
if __name__ == '__main__':
	read_file()
	#### Districting Problem
	print("\n********** Find a proper layer design *********************\n")
	w, m_I, m_O, best_rot_deg = get_best_layer_design()
	print("\n---- Set m_I = ",m_I,", m_O = ",m_O,", w = ",w," ----",sep = "")
	print("\n---- Set rotation degree = ", best_rot_deg, " ----",sep = "")
	print("\n********** Districting problem is done ********************")
	# find districting points in 1-layer
	district_points_1st = find1stLayerDistrictPoint(m_I,best_rot_deg)
	# find center of 1st layer
	best1stCenter = find1stLayerCenter()
	# find end points of districting line
	district_end_points_1st = find1stDistrictEndPoint(m_I,district_points_1st, best1stCenter)
	# find center of each sub-district in 2-layer design
	best2ndCenter = find2ndLayerCenter(m_I,district_end_points_1st)
	# distribute customer points to its corresponding district
	districted_customer_points_1st = find1stLayerCust(m_I,dataCust, district_end_points_1st,best2ndCenter)

	if(w == 2):
		# find districting points of each sub-district in 2nd layer design
		district_points_2nd = find2ndLayerDistrictPoint(m_I,m_O)
		# find end points of each sub-sub-district
		district_end_points_2nd = find2ndDistrictEndPoint(district_end_points_1st, best2ndCenter, district_points_2nd, m_I, m_O)
		# find center of each sub-sub-district
		best3rdCenter = find3rdLayerCenter(district_end_points_2nd, m_I, m_O)
		# distribute customer points to its corresponding district
		districted_customer_points_2nd = find2ndLayerCust(dataCust, district_end_points_2nd,m_I,m_O,best3rdCenter)
		


	#print("\n********** Start ring network design problem **************")
	cand_exch_point = []
	if(w == 1):
		# get candidate exchange point 
		cand_exch_point = getCandExchPoint(w,m_I,m_O,best2ndCenter, r, maxN, minN)
		print(cand_exch_point,end='\n\n')
		# calculate weight of each candidate exchange point
		cep_weight = getCepWeight(w,m_I,m_O,a,cand_exch_point,districted_customer_points_1st)
		# get exchange point of 1st layer
		exch_point_1st = getExchPoint(w,m_I,m_O,cep_weight, cand_exch_point,EPSILON)
		print(exch_point_1st)
		cost = test_show_exch_point_cost_1st(exch_point_1st)
		print('Exchange points\' cost = {}'.format(cost*(TIME_PERIOD_NUM-1)))
	else: # w = 2
		cand_exch_point = getCandExchPoint(w,m_I,m_O,best3rdCenter, r, maxN, minN)
		# calculate weight of each candidate exchange point
		cep_weight = getCepWeight(w,m_I,m_O,a,cand_exch_point, districted_customer_points_2nd)
		# get exchange point
		exch_point = getExchPoint(w,m_I,m_O,cep_weight, cand_exch_point,EPSILON)
		exch_point_2nd = exch_point
		exch_point_1st = []
		for i in range(len(exch_point)):
			n = len(exch_point[i])
			exch_point_1st.append(exch_point[i][n-2])
	print("\n********** Ring network design problem is done ************")

	### Visualization
	if(VISUALIZE):
		draw_map()

	if(w == 1):
		output_info_1(w,m_I,m_O,best_rot_deg,best1stCenter,best2ndCenter,district_end_points_1st,districted_customer_points_1st,exch_point_1st)
	else:
		output_info_2(w,m_I,m_O,best_rot_deg,best1stCenter,best2ndCenter,best3rdCenter,district_end_points_1st,district_end_points_2nd, districted_customer_points_2nd,exch_point_1st,exch_point_2nd)

	if(w == 1):
		output_testcase_1(districted_customer_points_1st,cand_exch_point,best2ndCenter)
	else:
		output_testcase_2(districted_customer_points_2nd,cand_exch_point,best3rdCenter)
