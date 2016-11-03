#!/bin/sh

for f in *.cpp; do
    name=$(echo $f | cut -f 1 -d '.')
    g++ $f -o  ../bin/$name
done
