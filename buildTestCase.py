import random
import os
#### Constant
SIDE_LENGTH = 100 # boundaries length
POINTS_SIDE_LEN = int(SIDE_LENGTH*0.9) # prevent the customer points lie on boundaries
INTERVAL = SIDE_LENGTH//20
CUSTOMER_NUM = 30

##### FOR DEBUG ##
random.seed(105) #
##### FOR DEBUG ##

#### open files
curPath = os.path.dirname(os.path.abspath(__file__))
fileBound = open(curPath+"/boundaries.txt", "w")
fileCust = open(curPath+"/customers.txt", "w")

#### build boundary points
# left_x, bottom_y, right_x, head_y
left_x = 0
right_x = SIDE_LENGTH
bottom_y = 0
head_y = SIDE_LENGTH
interval = SIDE_LENGTH//20
fileBound.write(str(left_x) + " "+str(right_x) + " "+str(bottom_y) + " "+str(head_y) + " " + str(interval) + "\n")

# make bottom line
for i in range(0,SIDE_LENGTH+1,INTERVAL):
	fileBound.write(str(i) + " 0")
	fileBound.write("\n")
# make left line
for i in range(INTERVAL,SIDE_LENGTH,INTERVAL):
	fileBound.write("0 "+str(i))
	fileBound.write("\n")
# make right line
for i in range(INTERVAL,SIDE_LENGTH,INTERVAL):
	fileBound.write(str(SIDE_LENGTH)+" "+str(i))
	fileBound.write("\n")
# make top line
for i in range(0,SIDE_LENGTH+1,INTERVAL):
	fileBound.write(str(i) + " " + str(SIDE_LENGTH))
	fileBound.write("\n")

for i in range(CUSTOMER_NUM):
	x = random.randrange(SIDE_LENGTH-POINTS_SIDE_LEN,POINTS_SIDE_LEN,INTERVAL)
	y = random.randrange(SIDE_LENGTH-POINTS_SIDE_LEN,POINTS_SIDE_LEN,INTERVAL)
	fileCust.write(str(x))
	fileCust.write(" ")
	fileCust.write(str(y))
	fileCust.write("\n")

#### close file
fileBound.close()
fileCust.close()
