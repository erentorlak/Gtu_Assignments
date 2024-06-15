CC=gcc
CFLAGS=-Wall
TARGET=hw1

# Target to build the program
all: $(TARGET) run

# Compile main.c into the executable
$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean the build directory
clean:
	rm -f $(TARGET) *.txt