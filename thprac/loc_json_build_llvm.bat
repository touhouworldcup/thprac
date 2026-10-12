@echo off
clang -I..\lib\yyjson -g -gcodeview -O3 -ffast-math -std=c++20 -march=native -o loc_json.exe loc_json.cpp -Wl,-INCREMENTAL:NO -Wl,-OPT:REF
