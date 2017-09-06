VERBOSE = 1

CXX = g++

LDFLAGS = -lpthread -lcurl -lz -lboost_system -lboost_filesystem
WINDOWS = false

ifeq ($(WINDOWS), true)
	LDFLAGS += -lopengl32 -lglew32 -lglfw3
else
	LDFLAGS += -lGL -lGLEW -lglfw
endif

CXXFLAGS = -std=c++14 -Wall -I ./libs/cpr/include -I ./libs/zipper

SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))
IMGUI = libs/imgui
IMGUISRC = $(wildcard $(IMGUI)/*.cpp)
IMGUIOBJ = $(patsubst $(IMGUI)/%.cpp,$(IMGUI)/%.o,$(IMGUISRC))

EXE = main

ifeq ($(WINDOWS), true)
	EXE := $(EXE).exe
endif

DEBUG = off


ifeq ($(DEBUG), on)
	CXXFLAGS += -g -DDEBUG
else
	CXXFLAGS += -O2
	LDFLAGS += -O2
endif

default: $(EXE)

build-submodules:
	git submodule update --init --recursive
	cd libs/zipper; mkdir -p build; cd build; cmake ..; make; \
	cd ..; cd ..; \
	cd cpr; mkdir -p build; cd build; cmake .. -DUSE_SYSTEM_CURL=ON -DBUILD_CPR_TESTS=OFF -DGENERATE_COVERAGE=OFF; make;

$(EXE): $(OBJECTS) $(IMGUIOBJ) ./libs/cpr/build/lib/libcpr.a ./libs/zipper/build/libZipper-static.a
	$(CXX) $^ -o $@ $(LDFLAGS)

$(IMGUI)/%.o: $(IMGUI)/%.cpp
	$(CXX) -c $< -o $@

clean:
	rm *.o
	rm $(IMGUI)/*.o
	rm $(EXE)

run:
	./$(EXE)
