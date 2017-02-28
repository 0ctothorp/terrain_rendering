CXX = g++

STATIC_LIBS = ./extlibs/libs/libGLEW.a ./extlibs/libs/libglfw3.a
DYNAMIC_LIBS_DIR = ./extlibs/lib
DYNAMIC_LIBS = -L$(DYNAMIC_LIBS_DIR) -lGL -lGLEW -lglfw
HEADERS = -I ./extlibs/include/

SRC = src
BUILD = build

default: $(BUILD)/main

$(BUILD)/main: $(BUILD)/main.o
	$(CXX) $(BUILD)/main.o -o $(BUILD)/main $(DYNAMIC_LIBS) -Wl,-rpath,$(DYNAMIC_LIBS_DIR)

$(BUILD)/main.o: $(SRC)/main.cpp
	mkdir -p $(BUILD)
	$(CXX) -c $(SRC)/main.cpp $(HEADERS) -o $(BUILD)/main.o -Werror -Wall -std=c++14

clean:
	rm $(BUILD)/*

run:
	$(BUILD)/main