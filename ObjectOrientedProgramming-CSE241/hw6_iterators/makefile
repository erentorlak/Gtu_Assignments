all: compile1 run1
d1: compile1 run1
d2: clean2 compile2 run2

clean1:
	@echo "-------------------------------------------"
	@echo "Removing compiled files of driver1..."
	@rm -f .o
	@rm -f test


compile1: driver.cpp DynamicArray.cpp 
	@echo "-------------------------------------------"
	@echo "Compiling driver1..."
	@g++ -std=c++11 driver.cpp DynamicArray.cpp -o test

run1:
	@echo "-------------------------------------------"
	@echo "Running the tests on driver1...."
	@echo "================================================================================="
	./test
	@echo "================================================================================="
	@echo "Completed tests on driver1...."	
clean2:
	@echo "-------------------------------------------"
	@echo "Removing compiled files driver2..."
	@rm -f .o

	@rm -f test2	
compile2: driver2.cpp tetromino.cpp tetrisarray.cpp tetrisvector.cpp tetrisadapter.cpp
	@echo "-------------------------------------------"
	@echo "Compiling driver2..."
	@g++ -std=c++11 driver2.cpp tetromino.cpp tetrisvector.cpp tetrisarray.cpp tetrisadapter.cpp -o test2	
run2:
	@echo "-------------------------------------------"
	@echo "Running the tests on driver2...."
	@echo "================================================================================="
	./test2
	@echo "================================================================================="
	@echo "Completed tests on driver2...."