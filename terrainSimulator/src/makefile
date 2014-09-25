#Makefile by Carl Stejmar, 2014-09-11

# Determine system and compiler
UNAME := $(shell uname)

ifeq ($(UNAME), Darwin) # Mac
CXX     = gcc
CCFLAGS = -g -std=c++11 -stdlib=libc++ -pedantic -Wall -Wextra
CFLAGS  = -g -std=c99 -pedantic -Wall -Wextra
LINK    = -g -shared-libgcc -lstdc++ -framework OpenGL -framework Cocoa
GLUTPATH = ../common/mac
GLUT	= ../common/mac/MicroGlut.m
endif
ifeq ($(UNAME), Linux)
CXX     = gcc
CCFLAGS = -g -std=c++0x -pedantic -Wall -Wextra -lGL -DGL_GLEXT_PROTOTYPES
CFLAGS  = -g -std=c99 -pedantic -Wall -Wextra -lGL -DGL_GLEXT_PROTOTYPES
LINK    = -g -shared-libgcc -lstdc++ -lm -lGL -DGL_GLEXT_PROTOTYPES
GLUTPATH = ../common/linux
GLUT	= ../common/linux/MicroGlut.c
endif

CCFILES = Main.cpp Skybox.cpp Camera.cpp TerrainGenerator.cpp

UTILS = ../common/GL_utilities.c ../common/VectorUtils3.c ../common/loadobj.c ../common/zpr.c ../common/LoadTGA.c $(GLUT)
OFILES = Main.o Skybox.o Camera.o TerrainGenerator.o ../common/GL_utilities.o ../common/loadobj.o ../common/LoadTGA.o ../common/VectorUtils3.o ../common/zpr.o $(GLUTPATH)/MicroGlut.o


main: $(OFILES)
	$(CXX) $(OFILES) $(LINK) -o main

..common/%.o: $(UTILS)
	$(CXX) -c $(UTILS) $(CFLAGS) $< 

%.o: %.cpp
	$(CXX) -c $(CCFILES) $(CCFLAGS) $<
#skybox.o: skybox.cpp
	#$(CXX) -c skybox.cpp $(CCFLAGS)
#Camera.o: Camera.cpp
	#$(CXX) -c Camera.cpp $(CCFLAGS)
#TerrainGenerator.o: TerrainGenerator.cpp
	#$(CXX) -c TerrainGenerator.cpp $(CCFLAGS)


clean:
	@ \rm -f *.o *.gch ../common/*.o ../common/mac/*.o ../common/linux/*.o main

zap: clean
	@ \rm -f *~ ../common/*~
