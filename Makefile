SFML_PATH=extlibs/SFML-2.4.2/
GLM_PATH=extlibs/glm-0.9.8.5/
GLEW_PATH=extlibs/glew-2.1.0/

CC=g++
CFLAGS=-Wall -Wextra -pedantic -O2 -Iinclude -I$(GLM_PATH) -I$(SFML_PATH)/include -I$(GLEW_PATH)/include -L$(GLEW_PATH)/lib -L$(SFML_PATH)/lib -std=c++11
tCFILES=$(wildcard src/*.cpp)
CFILES=$(tCFILES:src/%=%)
OFILES=$(CFILES:%.cpp=obj/%.o)
EXEC=flow

LIB=-lsfml-graphics -lsfml-window -lsfml-system -lGL -lGLEW

ifdef DEBUG
CFLAGS=-Wall -Wextra -pedantic -g -Iinclude -std=c++11
LIB=-lsfml-graphics-d -lsfml-window-d -lsfml-system-d -lGL -lGLEW
endif

.PHONY all:
.PHONY clean:
.PHONY cleanall:
.PHONY run:

all: bin/$(EXEC)

bin/$(EXEC): $(OFILES)
	mkdir -p bin
	$(CC) -o $@ $(CFLAGS) $(OFILES) $(LIB)

obj/%.o: src/%.cpp
	mkdir -p obj
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf obj/*

cleanall:
	rm -rf obj/* bin/$(EXEC)

run: bin/$(EXEC)
	export LD_LIBRARY_PATH="$(SFML_PATH)/lib;$(GLEW_PATH)/lib" ; bin/$(EXEC) rc/monkey.obj

run_gdb: bin/$(EXEC)
	export LD_LIBRARY_PATH="$(SFML_PATH)/lib;$(GLEW_PATH)/lib" ; gdb bin/$(EXEC) rc/monkey.obj

run_valgrind: bin/$(EXEC)
	export LD_LIBRARY_PATH="$(SFML_PATH)/lib;$(GLEW_PATH)/lib" ; valgrind --leak-check=full bin/$(EXEC) rc/monkey.obj


