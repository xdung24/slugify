#!/bin/bash

echo
echo "Running tests..."
echo
echo "Input: 'Hello World!'"
echo "Hello World!" | slugify
echo
echo "Input: 'Café Münster'"
echo "Café Münster" | slugify
echo
echo "Input: '北京 2024'"
echo "北京 2024" | slugify
echo
echo "Tests completed."