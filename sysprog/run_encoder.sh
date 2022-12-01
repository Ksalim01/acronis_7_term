#!/bin/bash

g++ -std=c++17 encoder.cpp -o encoder
./encoder $1
