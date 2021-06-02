test:
	g++ -std=c++17 main.cpp -o dvrp
all:
	python3 buildTestCase.py
	python3 drawMap.py
	g++ -std=c++17 main.cpp -o dvrp
clean:
	rm -f dvrp
