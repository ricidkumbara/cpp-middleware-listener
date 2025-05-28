@echo off
REM Set compiler paths - update if TDM-GCC is in a different folder
set CC=C:\TDM-GCC-64\bin\gcc.exe
set CXX=C:\TDM-GCC-64\bin\g++.exe

REM Create build directory if not exists
if not exist build (
    mkdir build
)

REM Move into build directory
cd build

REM Run CMake with MinGW Makefiles
cmake .. -G "MinGW Makefiles"

REM Build the project
cmake --build .

pause
