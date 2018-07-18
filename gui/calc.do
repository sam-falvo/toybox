#!/bin/bash
DEPS='calc.o events.o objects.o workstation.o'
redo-ifchange $DEPS
gcc -o $3 $DEPS $(pkg-config --libs sdl2)

