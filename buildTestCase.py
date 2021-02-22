import random
import os
#### Constant
SIDE_LENGTH = 20000
INTERVAL = 100
CUSTOMER_NUM = 100
DISTRICT_INTERVAL = INTERVAL*100
#### open files
curPath = os.path.dirname(os.path.abspath(__file__))
fileBound = open(curPath+"/boundaries.txt", "w")
fileCust = open(curPath+"/customers.txt", "w")
fileCustDistrict = open(curPath+"/cust_postal_num.txt", "w")
#### build boundary points
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

#### build customers points & customer district number
fileCustDistrict.write(str((SIDE_LENGTH//DISTRICT_INTERVAL)**2))
fileCustDistrict.write("\n")
fileCustDistrict.write(str(CUSTOMER_NUM))
fileCustDistrict.write("\n")
cnt = 0
for i in range(CUSTOMER_NUM):
	x = random.randrange(INTERVAL,SIDE_LENGTH,INTERVAL)
	y = random.randrange(INTERVAL,SIDE_LENGTH,INTERVAL)
	fileCust.write(str(x))
	fileCust.write(" ")
	fileCust.write(str(y))
	fileCust.write("\n")
	if(x//DISTRICT_INTERVAL != 0):
		fileCustDistrict.write(str((x//DISTRICT_INTERVAL)*SIDE_LENGTH//DISTRICT_INTERVAL+y//DISTRICT_INTERVAL)+"\n")
	else:
		fileCustDistrict.write(str(y//DISTRICT_INTERVAL)+"\n")
#### close file
fileBound.close()
fileCust.close()
fileCustDistrict.close()
