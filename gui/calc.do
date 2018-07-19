#!/bin/bash

DEPS='calc.o'
LIBS='libs/events/libevents.so libs/objects/libobjects.so libs/workstation/libworkstation.so'

redo-ifchange $DEPS $LIBS
gcc -o $3 $DEPS -Llibs/events -Llibs/objects -Llibs/workstation -lobjects -levents -lworkstation $(pkg-config --libs sdl2)
