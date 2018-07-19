#!/bin/bash
redo-ifchange workstation.o
gcc -shared -o $3 workstation.o $(pkg-config --libs sdl2)
