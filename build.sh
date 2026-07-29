#!/usr/bin/env bash

# Build script for Catherine Programming Language Interpreter

set -e

echo "Building Catherine Interpreter"

CXX=${CXX:-g++}
CXXFLAGS="-std=c++17 -O2 -Wall"
SRC_FILES="src/token.cpp src/lexer.cpp src/ast.cpp src/parser.cpp src/interpreter.cpp src/main.cpp"
TARGET="catherine"

echo "Compiling source files with $CXX..."
$CXX $CXXFLAGS $SRC_FILES -o $TARGET

if [ -f "$TARGET" ]; then
    echo "[+] Successfully built Catherine interpreter: ./$TARGET"
    echo "    Run Catherine files using: ./$TARGET <filename.rine>"
else
    echo "[-] Build failed."
    exit 1
fi
