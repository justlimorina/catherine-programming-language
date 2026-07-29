#!/usr/bin/env bash

set -e

mkdir -p build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

echo "Run Catherine files using: ./build/catherine <filename.rine>"
