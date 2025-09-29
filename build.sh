#!/bin/bash

# JsonSchemaSerializer Build Script

echo "JsonSchemaSerializer Build Script"
echo "================================="

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "Building project..."
make -j$(nproc)

# Run tests
echo "Running tests..."
./JsonSchemaSerializerTests

# Run example
echo "Running example..."
./JsonSchemaSerializerExample

echo "Build completed!"