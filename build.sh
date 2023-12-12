#!/bin/bash

mkdir -p build/
if type "gcc" > /dev/null; then
    echo "Building using gcc"
    gcc -o build/build-c-example.bin c-example.c
elif type "clang" > /dev/null; then
    echo "Building using clang"
    clang c-example.c -o build/build-c-example.bin
else
    echo "no C compiler found. You must build manually"
fi