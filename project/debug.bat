@echo off

REM 2017
REM call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
REM 2015 
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall" x64

devenv build/main.exe