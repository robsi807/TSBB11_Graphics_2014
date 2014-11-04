# set this variable to the director in which you saved the common files
commondir = ../common/

all: shadow

shadow: shadowmap.c $(commondir)LoadTGA.c $(commondir)VectorUtils3.c $(commondir)GL_utilities.c $(commondir)MicroGlut.c $(commondir)loadobj.c
	gcc -Wall -o shadow shadowmap.c -I$(commondir) $(commondir)LoadTGA.c $(commondir)VectorUtils3.c $(commondir)GL_utilities.c $(commondir)MicroGlut.c $(commondir)loadobj.c -DGL_GLEXT_PROTOTYPES -lGL -lX11 -lm

clean:
	rm -f shadow

