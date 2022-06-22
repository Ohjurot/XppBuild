@echo off

REM Setup argument
IF "%1"=="" SET bt="Release"
IF NOT "%1"=="" SET bt=%1

REM Run conan
conan install . --build missing -s build_type=%bt%

REM Run premake
"vendor/premake5" vs2022
