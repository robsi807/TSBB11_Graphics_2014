# set this variable to the director in which you saved the common files
commondir = ../common/

all: main

main: geom3d3stanford.c $(commondir)LoadTGA.c $(commondir)VectorUtils3.c $(commondir)GL_utilities.c $(commondir)linux/MicroGlut.c $(commondir)loadobj.c
	gcc -Wall -o main geom3d3stanford.c -I$(commondir) $(commondir)LoadTGA.c $(commondir)VectorUtils3.c $(commondir)GL_utilities.c $(commondir)linux/MicroGlut.c $(commondir)loadobj.c -DGL_GLEXT_PROTOTYPES -lGL -lX11 -lm -I$(commondir)

clean:
	rm -f main

