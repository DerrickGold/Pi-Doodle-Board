SHELL=/bin/bash
CC=gcc
CFLAGS=-std=gnu99 -O3 -DBMP_MODULE -DTOUCHSCREEN

LIBBAG=./libBAG
CFLAGS += -I $(LIBBAG)/libsrc/include

LDFLAGS = -lrt -lX11 $(LIBINCLUDES)
LDLIBS = $(LIBBAG)/lib/libbag.a


.PHONY: clean all

all: src/DrawingPi
	mv src/DrawingPi DrawingPi.bin

src/DrawingPi: src/DrawingPi.o src/button.o
src/DrawingPi.o: src/DrawingPi.c src/button.h
src/button.o: src/button.c src/button.h


clean:
	$(RM) DrawingPi.bin src/*.o


