import matplotlib.pyplot as plt

# arguments
FILE_NAME_READ = 'public_dataset/c101.txt'# read data from this file
FILE_NAME_WRITE1 = 'customers.txt' # write customers' data into this file
FILE_NAME_WRITE2 = 'boundaries.txt' # write boundaries' data into this file
CUSTOMER_NUM = 100 # first n customer we use (total 100 customers in dataset)

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

	# get min/max x,y in data, and set interval of boundary points
	interval = 2 # interval of creating boundary points
	max_x= max(x_list) + interval
	min_x = min(x_list) - interval
	max_y = max(y_list) + interval
	min_y = min(y_list) - interval
	
	
	# build boundaries.txt
	file = open(FILE_NAME_WRITE2,'w')
	file.write(str(min_x) + " "+str(max_x) + " "+str(min_y) + " "+str(max_y) + " " + str(interval) + "\n")
	for i in range(min_x,max_x+1,interval):# make bottom line
		file.write(str(i) + " " + str(min_y))
		file.write("\n")
	for i in range(min_y,max_y+1,interval): # make left line
		file.write(str(min_x)+" "+str(i))
		file.write("\n")
	for i in range(min_y,max_y+1,interval): # make right line
		file.write(str(max_x)+" "+str(i))
		file.write("\n")
	for i in range(min_x,max_x+1,interval): # make top line
		file.write(str(i) + " " + str(max_y))
		file.write("\n")
	file.close()
	
	
