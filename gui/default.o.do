#!/bin/bash
redo-ifchange $2.c
gcc -MD -MF $2.d -c -o $3 $2.c $(pkg-config --cflags sdl2)
read DEPS <$2.d
redo-ifchange ${DEPS#*:}

