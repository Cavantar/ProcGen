#pragma once 

#include "Includes.h"

class TerrainGenerator{
public:
	static vector<glm::vec4> generateTerrain(int dimensionLog2, glm::vec4 seed, float unitLength = 1.0f);
	static float sharpness;
private:
	static void doSquare(vector<float>& map, int mapWidth, int startIndex, int length, int passNumber);
	static void doDiamond(vector<float>& map, int mapWidth, int startIndex, int length, int passNumber);
	
	static void doSquarePass(vector<float>& map, int mapWidth, int length, int divisions);
	static void doDiamondPass(vector<float>& map, int mapWidth, int length, int divisions);

	static float getRandomOffset(int passNumber);
	static vector<glm::vec4> createVectorMap(vector<float>& map, int mapWidth, float unitLength);

	static void showMap(vector<float>& map, int length);
};