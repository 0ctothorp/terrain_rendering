CXX = g++

STATIC_LIBS = ./extlibs/libs/libGLEW.a ./extlibs/libs/libglfw3.a
DYNAMIC_LIBS_DIR = ./extlibs/lib
DYNAMIC_LIBS = -L$(DYNAMIC_LIBS_DIR) -lGL -lGLEW -lglfw
HEADERS = -I ./extlibs/include/

main: main.o
	$(CXX) -g build/main.o -o build/main $(DYNAMIC_LIBS) -Wl,-rpath,$(DYNAMIC_LIBS_DIR)

main.o: src/main.cpp
	$(CXX) -c src/main.cpp $(HEADERS) -o build/main.o -Werror -Wall -std=c++11

clean:
	rm build/*

run:
	./build/main