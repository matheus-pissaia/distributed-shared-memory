#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

echo "Compiling code..."

make rebuild

# Run the compiled code.
echo "Running code..."
./bin/a.out
