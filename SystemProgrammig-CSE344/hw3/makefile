CC = gcc
CFLAGS = -pthread -Wall -Wextra -std=c99

# Name of the executable
TARGET = parking_lot

# Default target
all: $(TARGET) run

# Link the object file to create the executable
$(TARGET): parking_lot.o
	$(CC) $(CFLAGS) -o $@ $^

# Compile the source file into an object file
parking_lot.o: parking_lot.c
	$(CC) $(CFLAGS) -c $<

# Clean up the compiled files
clean:
	rm -f *.o $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)
