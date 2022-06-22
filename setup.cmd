@echo off

REM Download all required files for development and testing
powershell -ExecutionPolicy ByPass -NoProfile -Command "& '%~dp0scripts\download_jdk.ps1'" %_args%
