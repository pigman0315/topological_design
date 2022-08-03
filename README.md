# topological_design
## How to prepare testing data
- Build&Preprocess self-made customers' data
	- `make all_private`
- Build&Preprocess public customers' data
	- First, put the directory of public dataset under **topological_design**
		```
		|- topological_design
			|- public_dataset
				|- c101.txt
				|- r101.txt
				|- ...
			|- Makefile
			|- buildTestCase.py
			|- ...
		```
	- And then, `make all_public`

## How to run
- **Prepare the testing data first**
- After preparing the testing data, `./main`

## Clean all txt file
- `make clean`

## Parameters to set
- buildTestCase.py
	- `SIDE_LENGTH`: side length of the map
	- `CUSTOMER_NUM`: total number of customers
- processTestCase.py
	- `FILE_NAME_READ`: file_path of the input file (e.g., dataset/r101.txt)
    - `FILE_NAME_WRITE1`: write customers' data into this file
    - `FILE_NAME_WRITE2`: write boundaries' data into this file
    - `CUSTOMER_NUM`: first n customer we use (total 100 customers in dataset)
    - `SIDE_LENGTH`: side length of the map	
- drawMap.py
    - `LENGTH_OF_MAP`: side length of the map
- main.cpp
    - `SPEED`: speed of the courier
