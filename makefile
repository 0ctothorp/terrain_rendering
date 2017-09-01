CXX = g++

LDFLAGS = -lpthread -lcurl
WINDOWS = false

ifeq ($(WINDOWS), true)
	LDFLAGS += -lopengl32 -lglew32 -lglfw3
else
	LDFLAGS += -lGL -lGLEW -lglfw
endif

CXXFLAGS = -std=c++11 -Wall -I~/libs/cpr/include

SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))
IMGUI = libs/imgui
IMGUISRC = $(wildcard $(IMGUI)/*.cpp)
IMGUIOBJ = $(patsubst $(IMGUI)/%.cpp,$(IMGUI)/%.o,$(IMGUISRC))

EXE = main

ifeq ($(WINDOWS), true)
	EXE := $(EXE).exe
endif

DEBUG = on


ifeq ($(DEBUG), on)
	CXXFLAGS += -g -DDEBUG
else
	CXXFLAGS += -O3
	LDFLAGS += -O3
endif


$(EXE): $(OBJECTS) $(IMGUIOBJ) ~/libs/cpr/build/lib/libcpr.a
	$(CXX) $^ -o $@ $(LDFLAGS)

$(IMGUI)/%.o: $(IMGUI)/%.cpp
	$(CXX) -c $< -o $@

clean:
	rm *.o
	rm $(IMGUI)/*.o
	rm $(EXE)

run:
	./$(EXE)
