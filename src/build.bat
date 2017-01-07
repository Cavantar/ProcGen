@echo off

IF "%1" == "ninja" GOTO NINJA

if not exist ..\build mkdir ..\build
pushd ..\build

set LibsDirectory=../libs

set IncludeDirectories= ^
	-I%LibsDirectory%/AntTweakBar/include ^
	-I%LibsDirectory%/glew-1.10.0/include ^
	-I%LibsDirectory%/freeglut/include ^
	-I%LibsDirectory%/glm ^
	-I%E:/Projekty/jpb

set LibraryDirectories= ^
-LIBPATH:%LibsDirectory%/glew-1.10.0/lib/Release/x64/ ^
-LIBPATH:%LibsDirectory%/AntTweakBar/lib/ ^
-LIBPATH:%LibsDirectory%/freeglut/lib/x64/ ^
-LIBPATH:E:/Projekty/jpb/lib/

set Libs= ^
opengl32.lib ^
glu32.lib ^
freeglut.lib ^
glew32.lib ^
AntTweakBar64.lib ^
jpb.lib ^
winmm.lib

set FilesToCompile= ^
../src/main.cpp ^
../src/Camera.cpp ^
../src/Chunk.cpp ^
../src/ChunkMap.cpp ^
../src/Functions.cpp ^
../src/Game.cpp ^
../src/GLSLShader.cpp ^
../src/InputManager.cpp ^
../src/Net.cpp ^
../src/TerrainGenerator.cpp ^
../src/TexturedQuad.cpp ^
../src/TextureModule.cpp ^
../src/Window.cpp

REM Zi(Generate Debug information), FC(Full Path To Source), O2(Fast Code)

set CompilerOptions=%Defines% /FC /Zi /EHsc /MD /MP /wd4503 /wd4312 /nologo /FeProcGen.exe %IncludeDirectories%
REM set CompilerOptions=%Defines% /FC /Zi /EHsc /MD /nologo /FeProcGen.exe %IncludeDirectories%
set LinkerOptions=/link %LibraryDirectories%

REM /SUBSYSTEM:windows
cl /Od %CompilerOptions% %FilesToCompile% %Libs% %LinkerOptions%

REM cd ../code
REM start "" nmake

popd

GOTO END

:NINJA

echo NinjaBuild
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall" amd64
ninja

:END
