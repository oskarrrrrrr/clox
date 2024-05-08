TARGET = clox
LIBS = -lm
CC = gcc
CFLAGS = -g -Wall
SRC_DIR = src
BUILD_DIR = build

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))
HEADERS = $(wildcard $(SRC_DIR)/*.h)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f $(BUILD_DIR)/*.o
	-rm -f $(TARGET)
