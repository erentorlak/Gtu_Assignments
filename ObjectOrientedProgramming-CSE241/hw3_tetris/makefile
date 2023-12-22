d1: compile1 run clean

d2: compile2 run clean	

compile1: driver1.cpp tetris.cpp tetromino.cpp tetris.hpp tetromino.hpp
	@echo "Compiling..."
	@echo "-------------------------------------------"
	@g++	-std=c++11 -g driver1.cpp tetris.cpp tetromino.cpp -o eren

compile2: driver2.cpp tetris.cpp tetromino.cpp  tetris.hpp tetromino.hpp
	@echo "Compiling..."
	@echo "-------------------------------------------"
	@g++	-std=c++11 -g driver2.cpp tetris.cpp tetromino.cpp -o eren

run:
	@echo "Running tests..."
	@echo "-------------------------------------------"
	@./eren


clean:
	@rm -f *.o
	@rm -f eren
	
