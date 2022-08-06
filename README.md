# topological_design
## Introduction
- Create private dataset
    - `buildTestCase.py`
    - After executing, it will generate 2 files
        - `customers.txt`
        - `boundaries.txt`
- Process public dataset
    - `processTestCase.py`
    - After executing, it will generate 2 files
        - `customers.txt`
        - `boundaries.txt`
    - public dataset: https://www.sintef.no/globalassets/project/top/vrptw/solomon/solomon-100.zip
- Do the districting problem (SP1)
    - `drawMap.py`
    - It needs 2 files to run
        - `customers.txt`
        - `boundaries.txt`
    - After executing, it will generate 7 files
        - `c_m_l.txt`
        - `c_w.txt`
        - `de.txt`
        - `dij.txt`
        - `lu.txt`
        - `pw.txt`
        - `sp1_result.txt`
- Do the main algorithm (SP2~SP4)
    - `main.cpp`
    - It needs 3 files to run
        - `sp1_result.txt`
        - `c_m_l.txt`
        - `c_w.txt`
    - After executing, you will get final results on the screen 
## How to prepare testing data
- Build&Preprocess self-made customers' data
	- input `make all_private` on terminal
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
	- And then, input `make all_public` on terminal

## How to run
- **Prepare the testing data first**
- After preparing the testing data, input `./main` on terminal

## Clean all txt file
- input `make clean` on terminal

## Parameters to set
- buildTestCase.py
	- `SIDE_LENGTH`: side length of the map
	- `CUSTOMER_NUM`: total number of customers
- processTestCase.py
	- `FILE_NAME_READ`: file_path of the input file (e.g., dataset/r101.txt)
    - `CUSTOMER_NUM`: first n customer we use (total 100 customers in dataset)
- drawMap.py
    - `LENGTH_OF_MAP`: side length of the map
    - `SPEED`: speed of the courier (must be the same as the one in main.cpp)
    - `H`: guarantee of sending time (must be the same as the one in main.cpp)
    - `SERV_COST`: service cost of each customers (must be the same as the one in main.cpp)
    - `TIME_PERIOD_NUM`: number of time period (must be the same as the one in main.cpp)
    - `VISUALIZE`: visualization of the final map (customer points/distriction/exchange points/boundaires)
    - `PEAK_CUSTOMER_THRESHOLD`:  when the number of customers (in all district) in the time period over the threshold, it is a  peak time period
    - `CUSTOMER_RATIO`: simulated customer ratio in each time period
- main.cpp
    - `SPEED`: speed of the courier (must be the same as the one in drawMap.cpp)
    - `H`: guarantee of sending time (must be the same as the one in drawMap.cpp)
    - `SERV_COST`: service cost of each customers (must be the same as the one in drawMap.cpp)
    - `time_period`: number of time period (must be the same as the one in drawMap.cpp)
    - `MAX_POSTAL_NUM`: total postal number (usually 2 x NUM_OF_DISTRICT)
    - `DELTA1`: delta difference of object 1 and object 2
    - `DELTA2`: delta difference of object 2 and object 3
