#!/bin/bash

EXEC="GameOfLife3D"

if [[ "$1" == "--help" || "$1" == "-h" ]]; then
    echo "Usage: ./exec.sh [MODE]"
    echo ""
    echo "Modes:"
    echo "  1             Run in real-time mode (Default)."
    echo "  2             Run in offline rendering mode (Exports frames to 'render/')."
    echo "  -h, --help    Show this help message and exit."
    echo ""
    echo "Examples:"
    echo "  ./exec.sh       # Compiles and runs in default real-time mode"
    echo "  ./exec.sh 2     # Compiles and runs the offline renderer"
    exit 0
fi

echo "🔨 Starting build process..."

mkdir -p build

cd build

cmake ..

make -j$(nproc)

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"

    cd ..
    
    echo "🚀 Starting $EXEC..."
    echo "----------------------------------------"

    ./build/$EXEC $@
else
    echo "❌ Build failed. The program was not launched."
fi