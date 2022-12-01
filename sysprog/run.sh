#!/bin/bash

rm logs
g++ -std=c++17 main.cpp -o main
sudo ./main $1 > logs 2>&1
