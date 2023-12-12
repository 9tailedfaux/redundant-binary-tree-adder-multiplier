#!/bin/bash

if [[ $# -ne 2 ]]; then 
    echo "Usage: ./run.sh [multiplicand] [multiplier]"
else
    ./build/build-c-example.bin $1 $2
fi