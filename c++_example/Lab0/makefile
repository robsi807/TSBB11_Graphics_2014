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

CCFILES = lab0.cpp
UTILS = ../common/GL_utilities.c ../common/VectorUtils3.c ../common/loadobj.c ../common/zpr.c ../common/LoadTGA.c $(GLUT)
OFILES = lab0.o ../common/GL_utilities.o ../common/loadobj.o ../common/LoadTGA.o ../common/VectorUtils3.o ../common/zpr.o $(GLUTPATH)/MicroGlut.o

main: $(OFILES)
	$(CXX) $(OFILES) $(LINK) -o main

cfiles: $(UTILS)
	$(CXX) -c $(UTILS) $(CFLAGS)

lab0.o: $(CCFILES)
	$(CXX) -c $(CCFILES) $(CCFLAGS)

clean:
	@ \rm -f *.o *.gch ../common/*.o ../common/mac/*.o ../common/linux/*.o main

zap: clean
	@ \rm -f *~ ../common/*~
