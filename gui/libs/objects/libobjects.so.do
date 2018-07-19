#!/bin/bash
redo-ifchange objects.o
gcc -shared -o $3 objects.o
