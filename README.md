# ProcGen

This is a project that I worked on between 2015-2016, it served as my dissertation. I've had lot's of fun writing this code, though it's as always, not my best.

Project currently builds only on Windows using provided build script - directly through vs compiler or indirectly through ninja.
I used Visual Studio 2016 community as a compiler with my trusty emacs as a text editor.

Program can generate textures and terrain based on definitions of fractal noises and rules of combining them - through expressions.
Generated content can be saved as a geometry file (.obj) or as a bitmap texture (.bmp).
I wrote simple expression interpreter (It's really bad as i had no prior knowledge of tokenizing, lexing and stuff), that adds really powerfull method of combining and testing different combinations of noises in real time.

Chunk generation is multithreaded, and I played a bit with simd optimizations.

My dissertation provides general overview of the functionality of the program - Unfortunately it's only in polish.

## Screenshots:

![Multifractal]   (/images/blend_multifractal.png)
![MountainTerrain](/images/mountainTerrain1.png)
![DirtWaterBase]  (/images/terrainDirtWaterBase2.png)
![StalAbstract]   (/images/terrainStalAbstract.png)
![Stalagmites]    (/images/terrainStalagmites.png)
![SinExample]     (/images/terrainSinExample.png)
![WorleyManhat]   (/images/terrainWorleyManhattan.png)
![BloodTiles]     (/images/bloodTilesTexture1.png)
![MarbleTiles]    (/images/marbleTexture1.bmp)
![TextureBlend]   (/images/testTextureBlend.bmp)
![SidePanel]      (/images/panel_boczny.png)
