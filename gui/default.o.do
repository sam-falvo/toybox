#!/bin/bash
redo-ifchange ./config.od $2.c

. ./config.od

gcc -MD -MF $2.d -I${INCDIR} -c -o $3 $2.c $(pkg-config --cflags sdl2)
read DEPS <$2.d
redo-ifchange ${DEPS#*:}

