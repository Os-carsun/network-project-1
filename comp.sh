#!/bin/sh

prefix="commands/"
postfix=".cpp"
for f in commands/*.cpp; do
    name=${f#$prefix}
    name=${name%$postfix}
    g++ $f -o  runtime/bin/$name
done
