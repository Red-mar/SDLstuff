@echo off

REM 2017
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
REM 2015
REM call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall" x64

REM -WX Treat warning as error;
REM D:\CProject\include
REM C:\Users\a\Desktop\git\SDLstuff\include

set CommonCompilerFlags=-MT -nologo -EHsc -Gm- -GR- -EHa- -Od -Oi -W4 -wd4201 -wd4100 -wd4189 -DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -FC -Z7 -Fmwin32_handmade.map -I D:\CProject\include

REM /LIBPATH: specifies the path

REM D:\CProject
REM C:\Users\a\Desktop\git\SDLstuff
set CommonLinkerFlags=-opt:ref /LIBPATH:"D:\CProject" include\SDL2.lib include\SDL2main.lib include\SDL2_image.lib include\SDL2_ttf.lib /SUBSYSTEM:CONSOLE

mkdir build
pushd build
REM 32-bit build
REM cl  %CommonCompilerFlags% ..\project\code\main.cpp /link -subsystem:windows,5.1 %CommonLinkerFlags%
REM 64-bit build
cl  %CommonCompilerFlags% ..\project\code\main.cpp /link %CommonLinkerFlags%
popd