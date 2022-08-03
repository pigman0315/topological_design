import matplotlib.pyplot as plt

# arguments
FILE_NAME_READ = 'public_dataset/r101.txt'# read data from this file
FILE_NAME_WRITE1 = 'customers.txt' # write customers' data into this file
FILE_NAME_WRITE2 = 'boundaries.txt' # write boundaries' data into this file
CUSTOMER_NUM = 25 # first n customer we use (total 100 customers in dataset)
SIDE_LENGTH = 100 # side length of the map
INTERVAL = SIDE_LENGTH//20 # INTERVAL of creating boundary points
# main
if __name__ == '__main__':


	# build custormers.txt
	file = open(FILE_NAME_READ)
	content = file.readlines()[9:9+CUSTOMER_NUM] # 9: starting line number with meaningful data
	x_list = []
	y_list = []
	for line in content:
		numbers = line.split(" ")
		while("" in numbers):
			numbers.remove("")
		x_list.append(int(numbers[1])) # value of x
		y_list.append(int(numbers[2])) # value of y
	file.close()
	file = open(FILE_NAME_WRITE1,'w')
	for i in range(len(x_list)):
		file.write(str(x_list[i])+" "+str(y_list[i])+"\n")
	file.close()
	
	
	# build boundaries.txt
	file = open(FILE_NAME_WRITE2,'w')
	for i in range(0,SIDE_LENGTH+1,INTERVAL):# make bottom line
		file.write(str(i) + " 0")
		file.write("\n")
	for i in range(INTERVAL,SIDE_LENGTH,INTERVAL): # make left line
		file.write("0 "+str(i))
		file.write("\n")
	for i in range(INTERVAL,SIDE_LENGTH,INTERVAL): # make right line
		file.write(str(SIDE_LENGTH)+" "+str(i))
		file.write("\n")
	for i in range(0,SIDE_LENGTH+1,INTERVAL): # make top line
		file.write(str(i) + " " + str(SIDE_LENGTH))
		file.write("\n")
	file.close()
	
	
