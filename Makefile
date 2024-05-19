# Directories
BUILD_DIR=build
OBJ_DIR=$(BUILD_DIR)/obj
BIN_DIR=$(BUILD_DIR)/elf

SRC_DIR=src
INCLUDE_DIR=include

CFLAGS= -Wall -Wextra -Wshadow -std=c++11 -O2 -g
CC=g++

TARGET=$(BUILD_DIR)/jvm

# Add your sources here
SOURCES= main.cpp \
		$(SRC_DIR)/ClassLoader.cpp \
		$(SRC_DIR)/ClassLoaderErrors.cpp \
		$(SRC_DIR)/ClassPrinter.cpp \
		$(SRC_DIR)/OpcodePrinter.cpp \
		$(SRC_DIR)/RunTimeExceptions.cpp \
		$(SRC_DIR)/Jvm.cpp \
		$(SRC_DIR)/OpcodePrinter.cpp \
		$(SRC_DIR)/RunTimeExceptions.cpp 
		


OBJ_NAMES=$(SOURCES:.cpp=.o)
OBJECTS=$(patsubst %,$(OBJ_DIR)/%,$(OBJ_NAMES))

$(TARGET): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) -o $@ $^

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) -I$(INCLUDE_DIR) $(CFLAGS) -c -o $@ $^


.PHONY: all clean

all: $(TARGET)

clean:
	rm -rf build
