#!/bin/sh

prefix="commands/"
postfix=".cpp"
mkdir -p runtime
mkdir -p runtime/bin
for f in commands/*.cpp; do
    name=${f#$prefix}
    name=${name%$postfix}
    g++ $f -o  runtime/bin/$name
done
cp /bin/ls runtime/bin/ls
cp /bin/cat runtime/bin/cat
