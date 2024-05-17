TARGET = clox
TEST_TARGET = run-tests
LIBS = -lm
CC = gcc
CFLAGS = -g -Wall -I$(SRC_DIR) -I$(TEST_DIR)
SRC_DIR = src
TEST_DIR = test
BUILD_DIR = build

.PHONY: default all clean

default: $(TARGET)
all: default

MAIN_OBJECT = $(BUILD_DIR)/main.o
TEST_OBJECT = $(BUILD_DIR)/test_main.o
TEST_HELPER_OBJECT = $(BUILD_DIR)/test.o
SRC_OBJECTS = $(filter-out $(MAIN_OBJECT), $(OBJECTS))

OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))
HEADERS = $(wildcard $(SRC_DIR)/*.h) $(wildcard $(TEST_DIR)/*.h)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test.o: $(TEST_DIR)/test.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

$(MAIN_OBJECT): $(SRC_DIR)/main.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_TARGET): $(TEST_OBJECT) $(SRC_OBJECTS) $(TEST_HELPER_OBJECT)
	$(CC) $(TEST_OBJECT) $(SRC_OBJECTS) $(TEST_HELPER_OBJECT) -Wall $(LIBS) -o $@

$(TEST_OBJECT): $(TEST_DIR)/main.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

clean:
	-rm -f $(BUILD_DIR)/*.o
	-rm -f $(TARGET)
	-rm -f $(TEST_TARGET)
