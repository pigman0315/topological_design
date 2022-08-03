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
    - `CUSTOMER_NUM`: first n customer we use (total 100 customers in dataset)
    - `SIDE_LENGTH`: side length of the map	
- drawMap.py
    - `LENGTH_OF_MAP`: side length of the map
- main.cpp
    - `SPEED`: speed of the courier
