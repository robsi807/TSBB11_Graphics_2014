#Makefile by Carl Stejmar, 2014-09-11

COMMONDIR = ../common/
BOOSTDIR = ../../boost_1_57_0/

# Determine system and compiler
UNAME := $(shell uname)

GRASS=1
PLANTS=0
BIRDS=0
PERLIN=1
LOWGRAPHICS=0

PATCH_SIZE=512
PATCH_OVERLAP=PATCH_SIZE/4
GRID_BEGIN_SIZE=9

ifeq ($(UNAME), Darwin) # Mac
CXX     = gcc
DFLAGS = -DGRASS=$(GRASS) -DPLANTS=$(PLANTS) -DPERLIN=$(PERLIN) -DPATCH_OVERLAP=$(PATCH_OVERLAP) -DPATCH_SIZE=$(PATCH_SIZE) -DGRID_BEGIN_SIZE=$(GRID_BEGIN_SIZE) -DLOWGRAPHICS=$(LOWGRAPHICS) -DBIRDS=$(BIRDS)
CCFLAGS = -g -std=c++11 -stdlib=libc++ -pedantic -Wall -Wextra -I$(COMMONDIR) -I$(BOOSTDIR) $(DFLAGS)
CFLAGS  = -g -std=c99 -pedantic -Wall -Wextra -I$(COMMONDIR)
LINK    = -g -shared-libgcc -lstdc++ -framework OpenGL -framework Cocoa -lpthread -I$(COMMONDIR) -I$(BOOSTDIR)
GLUTPATH = $(COMMONDIR)mac
GLUT	= $(COMMONDIR)MicroGlut.m
endif
ifeq ($(UNAME), Linux)
CXX      = gcc
DFLAGS   = -DGRASS=$(GRASS) -DPLANTS=$(PLANTS) -DPERLIN=$(PERLIN) -DPATCH_OVERLAP=$(PATCH_OVERLAP) -DPATCH_SIZE=$(PATCH_SIZE) -DGRID_BEGIN_SIZE=$(GRID_BEGIN_SIZE) -DBIRDS=$(BIRDS) -DLOWGRAPHICS=$(LOWGRAPHICS)
CCFLAGS  = -g -std=c++0x -lGL -DGL_GLEXT_PROTOTYPES -I$(COMMONDIR) -I$(BOOSTDIR) $(DFLAGS)
CFLAGS   = -g -std=c99 -lGL -DGL_GLEXT_PROTOTYPES -I$(COMMONDIR)
LINK     = -g -shared-libgcc -lstdc++ -lm -lGL -lpthread -DGL_GLEXT_PROTOTYPES -lX11 -I$(COMMONDIR) -I$(BOOSTDIR)
GLUTPATH = $(COMMONDIR)linux
GLUT	 = $(COMMONDIR)MicroGlut.c
endif


CCFILES = Main.cpp Skybox.cpp Camera.cpp Frustum.cpp TerrainPatch.cpp World.cpp PerlinPatchGenerator.cpp PerlinPatchGeneratorFast.cpp ValuePatchGenerator.cpp Blender.cpp Plant.cpp Boid.cpp Evader.cpp Chaser.cpp ManageChasersAndEvaders.cpp
UTILS = $(COMMONDIR)GL_utilities.c $(COMMONDIR)VectorUtils3.c $(COMMONDIR)loadobj.c $(COMMONDIR)zpr.c $(COMMONDIR)LoadTGA.c $(GLUT)
OFILES = Main.o Skybox.o Camera.o Frustum.o TerrainPatch.o World.o PerlinPatchGenerator.o PerlinPatchGeneratorFast.o ValuePatchGenerator.o Blender.o Plant.o Boid.o Evader.o Chaser.o ManageChasersAndEvaders.o $(COMMONDIR)GL_utilities.o $(COMMONDIR)loadobj.o $(COMMONDIR)LoadTGA.o $(COMMONDIR)VectorUtils3.o $(COMMONDIR)zpr.o $(GLUTPATH)/MicroGlut.o


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
