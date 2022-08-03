test:
	g++ main.cpp -o main
all_public:
	python3 processTestCase.py
	python3 drawMap.py
	g++ main.cpp -o main
all_private:
	python3 buildTestCase.py
	python3 drawMap.py
	g++ main.cpp -o main
clean:
	rm -f main
