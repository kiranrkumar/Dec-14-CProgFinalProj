# Makefile - Mac OS/Linux
# Synthesizer
# - Final Project - Ryan Edwards and Kiran Kumar
# 

CC      = gcc
CFLAGS  = -g -std=c99 -I/usr/local/Cellar/portaudio/19.20140130/include -I/usr/local/Cellar/libsndfile/1.0.25/include -L/usr/local/lib -Wall -lportaudio
EXE  = synthesizer
SRCS = sigProcessing.c main.c
HDRS = sigProcessing.h

$(EXE):	$(SRCS) $(HDRS)
	$(CC) $(CFLAGS) $(SRCS) -o $@

clean:
	rm -f *~ core $(EXE) *.o
	rm -rf *.dSYM
