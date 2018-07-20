#!/bin/bash

OBJ=$2
SRC=${OBJ%.o}.c
DEP=${OBJ%.o}.d

gcc -MD -MF $DEP -c -o $3 $SRC
read DEPS <$DEP
redo-ifchange ${DEPS#*:}

