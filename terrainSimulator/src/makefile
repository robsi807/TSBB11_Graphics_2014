#Makefile by Carl Stejmar, 2014-09-11

COMMONDIR = ../common/
# Determine system and compiler
UNAME := $(shell uname)

ifeq ($(UNAME), Darwin) # Mac
CXX     = gcc
CCFLAGS = -g -std=c++11 -stdlib=libc++ -pedantic -Wall -Wextra -I$(COMMONDIR)
CFLAGS  = -g -std=c99 -pedantic -Wall -Wextra -I$(COMMONDIR)
LINK    = -g -shared-libgcc -lstdc++ -framework OpenGL -framework Cocoa -I$(COMMONDIR)
GLUTPATH = $(COMMONDIR)mac
GLUT	= $(COMMONDIR)MicroGlut.m
endif
ifeq ($(UNAME), Linux)
CXX     = gcc
CCFLAGS = -g -std=c++0x -lGL -DGL_GLEXT_PROTOTYPES -I$(COMMONDIR)
CFLAGS  = -g -std=c99 -lGL -DGL_GLEXT_PROTOTYPES -I$(COMMONDIR)
LINK    = -g -shared-libgcc -lstdc++ -lm -lGL -lpthread -DGL_GLEXT_PROTOTYPES -lX11 -I$(COMMONDIR)
GLUTPATH = $(COMMONDIR)linux
GLUT	= $(COMMONDIR)MicroGlut.c
endif

CCFILES = Main.cpp Skybox.cpp Camera.cpp Frustum.cpp TerrainPatch.cpp World.cpp PerlinPatchGenerator.cpp ValuePatchGenerator.cpp LinearBlender.cpp Sun.cpp
UTILS = $(COMMONDIR)GL_utilities.c $(COMMONDIR)VectorUtils3.c $(COMMONDIR)loadobj.c $(COMMONDIR)zpr.c $(COMMONDIR)LoadTGA.c $(GLUT)
OFILES = Main.o Skybox.o Camera.o Frustum.o TerrainPatch.o World.o PerlinPatchGenerator.o ValuePatchGenerator.o LinearBlender.o Sun.o $(COMMONDIR)GL_utilities.o $(COMMONDIR)loadobj.o $(COMMONDIR)LoadTGA.o $(COMMONDIR)VectorUtils3.o $(COMMONDIR)zpr.o $(GLUTPATH)/MicroGlut.o


main: $(OFILES)
	$(CXX) $(OFILES) $(LINK) -o main

..common/%.o: $(UTILS)
	$(CXX) -c $(UTILS) $(CFLAGS) $< 

%.o: %.cpp
	$(CXX) -c $(CCFILES) $(CCFLAGS) $<

clean:
	@ \rm -f *.o *.gch ../common/*.o ../common/mac/*.o ../common/linux/*.o main

zap: clean
	@ \rm -f *~ ../common/*~
