CXX = g++

DYNAMIC_LIBS = -lGL -lGLEW -lglfw -lgdal
FLAGS = -std=c++14 -Wall

BUILD = build
SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp,$(BUILD)/%.o,$(SOURCES))
LIBS = libs
IMGUI = $(LIBS)/imgui
STB = $(LIBS)/stb
IMGUISRC = $(wildcard $(IMGUI)/*.cpp)
IMGUIOBJ = $(patsubst $(IMGUI)/%.cpp,$(IMGUI)/%.o,$(IMGUISRC))
SHADERS = shaders

EXE = main

$(EXE): $(OBJECTS) $(IMGUIOBJ)
	$(CXX) $^ -o $@ $(DYNAMIC_LIBS)

$(BUILD)/%.o: %.cpp
	$(CXX) -c $< -o $@ $(FLAGS) -g

$(IMGUI)/%.o: $(IMGUI)/%.cpp
	$(CXX) -c $< -o $@ -Wall

clean:
	rm $(BUILD)/*
	rm $(IMGUI)/*.o
	rm main

run:
	./main