SHELL = /bin/sh

CXX	 := -gcc
ERRFLAGS := -pedantic-errors -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align \
			-Wunused -Woverloaded-virtual -Wpedantic -Wconversion -Wsign-conversion -Wmisleading-indentation \
			-Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wuseless-cast \
			-Wdouble-promotion -Wformat=2
CXXFLAGS := -std=c++20 $(ERRFLAGS)
LDFLAGS  := -L/usr/lib -lstdc++ -lfmt
TEST_LDFLAGS  := -lm
BUILD	:= ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
DEBUG 	 := $(APP_DIR)/debug
RELEASE  := $(APP_DIR)/release
TARGET   := player
INCLUDE  := -Iinclude/
SRC	 := $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)
TEST := $(wildcard test/*.cpp) $(wildcard test/**/*.cpp)
TEST_SRC := $(filter-out src/main.cpp, $(SRC)) $(TEST)

OBJECTS  := $(SRC:%.cpp=$(OBJ_DIR)/%.o)
TEST_OBJ := $(TEST_SRC:%.cpp=$(OBJ_DIR)/%.o)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@ $(LDFLAGS)

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS)

.PHONY: all build clean debug release test

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

test: CXXFLAGS += -O2
test: LDFLAGS += $(TEST_LDFLAGS)
test: APP_DIR := $(RELEASE)

## FIXME This does not work for some reason ##
test: SRC := $(TEST_SRC)
test: OBJECTS := $(TEST_OBJ)
## ////////////////////////////////// ##

test: run
test: all
# test: build $(APP_DIR)/$(TARGET)

# $(OBJ_DIR)/%.o: %.cpp
	# @mkdir -p $(@D)
	# $(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@ $(TEST_LDFLAGS)

# $(APP_DIR)/$(TARGET): $(TEST_OBJ)
	# @mkdir -p $(@D)
	# $(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(TEST_LDFLAGS)


run: $(APP_DIR)/$(TARGET)
	$(APP_DIR)/$(TARGET)

debug: CXXFLAGS += -DDEBUG -g
debug: APP_DIR := $(DEBUG)
debug: all

release: CXXFLAGS += -O2
release: APP_DIR := $(RELEASE)
release: all

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*
