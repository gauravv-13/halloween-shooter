# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE

# Linker flags
LDFLAGS = -L/opt/homebrew/lib -lSDL2 -lSDL2_image

# Directories
SRC_DIR = game
LIB_DIR = lib
INC_DIR = include
RES_DIR = resources

# Source files
CLIENT_SRC = $(SRC_DIR)/client.c
MAIN_SRC = $(SRC_DIR)/main_game.c
NETWORK_SRC = $(LIB_DIR)/network.c

# Object files
CLIENT_OBJ = $(SRC_DIR)/client.o
MAIN_OBJ = $(SRC_DIR)/main_game.o
NETWORK_OBJ = $(LIB_DIR)/network.o

# Executable name
EXECUTABLE = main_game

# Default target
all: $(EXECUTABLE)

# Build the executable
$(EXECUTABLE): $(MAIN_OBJ) $(CLIENT_OBJ) $(NETWORK_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compile source files into object files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/common.h
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_DIR)/%.o: $(LIB_DIR)/%.c $(INC_DIR)/common.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(SRC_DIR)/*.o $(LIB_DIR)/*.o $(EXECUTABLE)

.PHONY: all clean
