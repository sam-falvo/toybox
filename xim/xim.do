#!/bin/bash
redo-ifchange video-driver.o
gcc -o $3 video-driver.o $(pkg-config --libs sdl2)
