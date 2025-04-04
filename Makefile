# Executable name
TARGET = julia

# Compiler and flags
CC = gcc
CFLAGS = -fopenmp -lSDL2 -Wall -Wextra -O2

# Source files in src/
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

# Libraries (rawdraw uses X11, GL, math)
LIBS = -lX11 -lGL -lm -lSDL2 -fopenmp

# Default target
all: $(TARGET)

# Link object files
$(TARGET): $(OBJ)
	$(CC) -O2 -o $@ $^ $(LIBS)

# Compile .c to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

rebuild: clean all

# Clean
clean:
	rm -f $(TARGET) src/*.o


