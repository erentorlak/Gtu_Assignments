CC=gcc
CFLAGS=-Wall
TARGET=hw2

# Target to build the program
all: $(TARGET) run clean

# Compile main.c into the executable
$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

# Run the program
run: $(TARGET)
	./$(TARGET) 12
	

# Clean the build directory
clean:
	rm -f $(TARGET)