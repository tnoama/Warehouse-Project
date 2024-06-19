CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -Weffc++ -Iinclude
SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin
BUILD_DIR = bin
EXECUTABLE = bin/warehouse
SOURCE_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECT_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCE_FILES))

all: $(EXECUTABLE)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(EXECUTABLE): $(OBJECT_FILES)
	$(CXX) $(CXXFLAGS) $^ -o $@

.PHONY: clean

clean:
	rm -rf $(BIN_DIR)
