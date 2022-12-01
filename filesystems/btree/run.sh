#!/bin/bash

rm a.out
# g++ -std=c++17 -g -fsanitize=address main.cpp
# ./a.out
g++ main.cpp
valgrind ./a.out
