@echo off
color 0A
title TODO App - GUI Version Compiler

echo ============================================
echo    TODO APP - GUI VERSION COMPILER
echo ============================================

REM Compile sqlite3.c as C
gcc -c sqlite3.c -o sqlite3.o

REM Compile GUI C++ code and link with sqlite3.o
g++ TodoApp_GUI.cpp TaskQueue.cpp Database.cpp sqlite3.o -o TodoApp_GUI.exe ^
    -I. -mwindows -lcomctl32 -std=c++11 -static-libgcc -static-libstdc++

if errorlevel 1 (
    echo.
    echo [ERROR] Compilation failed!
    pause
    exit /b
)

echo.
echo [OK] Compilation successful!
echo Run: TodoApp_GUI.exe
pause
