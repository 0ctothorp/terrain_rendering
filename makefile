CXX = g++

DYNAMIC_LIBS = -lGL -lGLEW -lglfw
FLAGS = -std=c++11 -Wextra -Wall

BUILD = build
SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp,$(BUILD)/%.o,$(SOURCES))
SHADERS = shaders

main: $(OBJECTS)
	$(CXX) $^ -o main $(DYNAMIC_LIBS)

$(BUILD)/%.o: %.cpp
	$(CXX) -c $^ -o $@ $(FLAGS)

clean:
	rm $(BUILD)/*
	rm main

run:
	./main