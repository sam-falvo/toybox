#!/bin/bash
redo-ifchange events.o
gcc -shared -o $3 events.o
