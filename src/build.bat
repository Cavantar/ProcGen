@echo off

IF "%1" == "ninja" GOTO NINJA

if not exist ..\build mkdir ..\build
pushd ..\build

set UnityBuild=False

set LibsDirectory=E:/Libs

set IncludeDirectories= ^
    	-I%LibsDirectory%/AntTweakBar/include ^
	-I%LibsDirectory%/glew-1.10.0/include ^
	-I%LibsDirectory%/freeglut/include ^
	-I%LibsDirectory%/glm

set LibraryDirectories= ^
-LIBPATH:%LibsDirectory%/glew-1.10.0/lib/Release/x64/ ^
-LIBPATH:%LibsDirectory%/AntTweakBar/lib/ ^
-LIBPATH:%LibsDirectory%/freeglut/lib/x64/ 

set Libs= ^
opengl32.lib ^
glu32.lib ^
freeglut.lib ^
glew32.lib ^
AntTweakBar64.lib ^
winmm.lib 

if "%UnityBuild%" == "True" (
echo Unity Build

set FilesToCompile=..\code\main.cpp
set Defines=%Defines% /DUNITY_BUILD
)

if "%UnityBuild%" == "False" (
echo NonUnity Build
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
../src/Noise.cpp ^
../src/TerrainGenerator.cpp ^
../src/TexturedQuad.cpp ^
../src/Window.cpp 
)

REM Zi(Generate Debug information), FC(Full Path To Source), O2(Fast Code)

set CompilerOptions=%Defines% /FC /Zi /EHsc /MD /MP /wd4503 /nologo /FeProcGen.exe %IncludeDirectories% 
set LinkerOptions=/link %LibraryDirectories%

REM /SUBSYSTEM:windows 
cl %CompilerOptions% %FilesToCompile% %Libs% %LinkerOptions% 

REM cd ../code
REM start "" nmake

popd

GOTO END 

:NINJA

echo NinjaBuild
ninja

:END

