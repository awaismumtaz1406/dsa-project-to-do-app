@echo off
color 0A
title TODO App - Console Version Compiler

echo ============================================
echo    TODO APP - CONSOLE VERSION COMPILER
echo ============================================

REM Compile sqlite3.c as C
gcc -c sqlite3.c -o sqlite3.o

REM Compile C++ code and link with sqlite3.o
g++ TodoApp_Console.cpp TaskQueue.cpp Database.cpp sqlite3.o -o TodoApp_Console.exe -std=c++11 -static-libgcc -static-libstdc++

if errorlevel 1 (
    echo.
    echo [ERROR] Compilation failed!
    pause
    exit /b
)

echo.
echo [OK] Compilation successful!
echo Run: TodoApp_Console.exe
pause
