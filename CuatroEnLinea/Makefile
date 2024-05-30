CC = g++
CFLAGS = -Iinclude -Wall -std=c++11 -g
LDFLAGS = 

# Directorios
SRC_DIR = src
BIN_DIR = bin

#Targets
all: server client

server: $(SRC_DIR)/server.cpp
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/server $(SRC_DIR)/server.cpp $(LDFLAGS)

client: $(SRC_DIR)/client.cpp
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/client $(SRC_DIR)/client.cpp $(LDFLAGS)

clean:
	rm -f $(BIN_DIR)/*