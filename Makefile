# Makefile - Mac OS/Linux
# Synthesizer
# - Final Project - Ryan Edwards and Kiran Kumar
# 

CC      = gcc -D__MACOSX_CORE__
CFLAGS  = -std=c99 -lportaudio -framework OpenGL -framework GLUT -lSOIL -Wno-deprecated
EXE  = synthesizer
SRCS = sigProcessing.c main.c
HDRS = sigProcessing.h

$(EXE):	$(SRCS) $(HDRS)
	$(CC) $(CFLAGS) $(SRCS) -o $@

clean:
	rm -f *~ core $(EXE) *.o
	rm -rf *.dSYM
