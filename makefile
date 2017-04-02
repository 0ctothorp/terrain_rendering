CXX = g++

DYNAMIC_LIBS = -lGL -lGLEW -lglfw
FLAGS = -std=c++11 -Wextra -Wall

BUILD = build
SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp,$(BUILD)/%.o,$(SOURCES))
SHADERS = shaders

main: $(OBJECTS)
	$(CXX) $^ -o main $(DYNAMIC_LIBS)

$(BUILD)/%.o: %.cpp %.hpp
	$(CXX) -c $< -o $@ $(FLAGS) -g

$(BUILD)/main.o: main.cpp
	$(CXX) -c $< -o $@ $(FLAGS) -g	

clean:
	rm $(BUILD)/*
	rm main

run:
	./main