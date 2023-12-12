#!/bin/bash

gcc -o build/build-c-example.bin c-example.c
clang c-example.c -o build/build-c-example.bin