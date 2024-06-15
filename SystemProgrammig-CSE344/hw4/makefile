CC = gcc
CFLAGS = -Wall -pthread
TARGET = 210104004090_main

all: $(TARGET)

$(TARGET): 210104004090_main.o
	$(CC) $(CFLAGS) -o $(TARGET) 210104004090_main.o

210104004090_main.o: 210104004090_main.c
	$(CC) $(CFLAGS) -c 210104004090_main.c

clean:
	rm -f $(TARGET) 210104004090_main.o

test: test1 test2 test3

test1: $(TARGET)
	@echo "Running Test 1 with Valgrind: buffer size=10, number of workers=10"
	valgrind --leak-check=full ./210104004090_main 10 10 ../testdir/src/libvterm ../tocopy

test2: $(TARGET)
	@echo "Running Test 2: buffer size=10, number of workers=4"
	./210104004090_main 10 4 ../testdir/src/libvterm/src ../toCopy

test3: $(TARGET)
	@echo "Running Test 3: buffer size=10, number of workers=10"
	./210104004090_main 10 10 ../testdir ../toCopy

.PHONY: all clean test test1 test2 test3
