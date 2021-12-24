@echo off

clang++ -O2 -Werror main.cpp -o main.exe || exit /b
clang++ -g main.cpp -o debug.exe
main
