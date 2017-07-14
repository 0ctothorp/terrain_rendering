CXX = g++

LDFLAGS = -lGL -lGLEW -lglfw -lgdal
CXXFLAGS = -std=c++14 -Wall

SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))
IMGUI = libs/imgui
IMGUISRC = $(wildcard $(IMGUI)/*.cpp)
IMGUIOBJ = $(patsubst $(IMGUI)/%.cpp,$(IMGUI)/%.o,$(IMGUISRC))

EXE = main

DEBUG = off


ifeq ($(DEBUG), on)
	CXXFLAGS += -g -DDEBUG
else
	CXXFLAGS += -O3
	LDFLAGS += -O3
endif


$(EXE): $(OBJECTS) $(IMGUIOBJ)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(IMGUI)/%.o: $(IMGUI)/%.cpp
	$(CXX) -c $< -o $@ -Wall

clean:
	rm *.o
	rm $(IMGUI)/*.o
	rm $(EXE)

run:
	./$(EXE)