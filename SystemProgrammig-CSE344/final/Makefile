CC = gcc
CFLAGS =  -lpthread -pthread
LDFLAGS = -lm
SERVER = PideShop
CLIENT = HungryVeryMuch
SERVER_SRC = PideShop.c
CLIENT_SRC = HungryVeryMuch.c
SERVER_PORT = 8099
COOK_POOL_SIZE = 4
DELIVERY_POOL_SIZE = 6
DELIVERY_SPEED = 1
CLIENT_COUNT = 15
TOWN_SIZE_P = 10
TOWN_SIZE_Q = 20

all: $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_SRC) $(LDFLAGS)

$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_SRC) $(LDFLAGS)

run-server: $(SERVER)
	./$(SERVER) $(SERVER_PORT) $(COOK_POOL_SIZE) $(DELIVERY_POOL_SIZE) $(DELIVERY_SPEED)

run-client: $(CLIENT)
	./$(CLIENT) 127.0.0.1 $(SERVER_PORT) $(CLIENT_COUNT) $(TOWN_SIZE_P) $(TOWN_SIZE_Q)

do: all run-server
	

clean:
	rm -f $(SERVER) $(CLIENT) *.txt

.PHONY: all run-server run-client clean
