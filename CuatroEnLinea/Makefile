CC = g++
CFLAGS = -Iinclude -Wall -std=c++11 -g
LDFLAGS = 

# Directorios
SRC_DIR = src
BIN_DIR = bin

#Targets
all: server client

server: $(BIN_DIR)/server.cpp
	$(CC) $(CFLAGS) $(SRC_DIR)/server.cpp -o $(BIN_DIR)/server $(LDFLAGS)

client: $(BIN_DIR)/client.cpp
	$(CC) $(CFLAGS) $(SRC_DIR)/client.cpp -o $(BIN_DIR)/client $(LDFLAGS)

clean:
	rm -f $(BIN_DIR)/*