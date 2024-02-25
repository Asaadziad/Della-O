#!/usr/bin/bash

mkdir -p bin

clang++  src/*.cpp -o bin/A  -D DEBUG_FLAG
