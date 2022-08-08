@echo off

set CLANG=clang++

set FLAGS=-O0 -D _DEBUG -g -gcodeview -Wall -Wpedantic -Wno-c++20-designator -Wno-language-extension-token

set LIBDIRS=-L../dependencies/GLFW/lib/

set LIBS=-luser32 -lshell32 -lgdi32 -lopengl32 -lglfw3_mt

set INCLUDEDIRS=-I./dependencies/GLFW/include

set DEFINES=-D_MT

if not exist build mkdir build
cd ./build
if not exist intermediates mkdir intermediates
cd ./intermediates

for /R ../../src/ %%I in (*.cpp) do %CLANG% %INCLUDEDIRS% -c %%I %DEFINES% %FLAGS% -o ./%%~nI.o || exit

cd ..

%CLANG% %LIBDIRS% %LIBS% -g ./intermediates/*.o -o opengl_test.exe