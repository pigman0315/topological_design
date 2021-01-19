import random
import os
#### Constant
SIDE_LENGTH = 20000
INTERVAL = 100
CUSTOMER_NUM = 300
#### open files
curPath = os.path.dirname(os.path.abspath(__file__))
fileBound = open(curPath+"/boundaries.txt", "w")
fileCust = open(curPath+"/customers.txt", "w")
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

#### build custormers points
for i in range(CUSTOMER_NUM):
	fileCust.write(str(random.randrange(INTERVAL,SIDE_LENGTH,INTERVAL)))
	fileCust.write(" ")
	fileCust.write(str(random.randrange(INTERVAL,SIDE_LENGTH,INTERVAL)))
	fileCust.write("\n")
#### close file
fileBound.close()
fileCust.close()