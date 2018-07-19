#!/bin/bash
DEFAULT_DIR=$(cd $(dirname ${BASH_SOURCE}) && pwd)
redo-ifchange ${DEFAULT_DIR}/../config.od $2.c

. ${DEFAULT_DIR}/../config.od

gcc -MD -MF $2.d -I${INCDIR} -fpic -c -o $3 $2.c $(pkg-config --cflags sdl2)
read DEPS <$2.d
redo-ifchange ${DEPS#*:}

