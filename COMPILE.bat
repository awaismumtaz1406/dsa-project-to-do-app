@echo off
echo Compiling CONSOLE version...

g++ main.cpp TaskQueue.cpp Database.cpp sqlite3.c ^
-o TodoApp.exe ^
-I. -std=c++11

if errorlevel 1 (
    echo COMPILATION FAILED
    pause
    exit /b
)

echo DONE
pause
