#!/bin/bash
redo-ifchange calc
D=$(cd $(dirname $BASH_SOURCE) && pwd)
echo $D >&2
L=$D/libs/events:$D/libs/objects:$D/libs/workstation
LD_LIBRARY_PATH=$L $D/calc
