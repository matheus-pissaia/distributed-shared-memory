#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

echo "Running tests..."

gcc tests/main.c api/api.c -o ./bin/test

./bin/test
