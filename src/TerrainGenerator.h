#pragma once

#include "Includes.h"

class TerrainGenerator{
public:
  static std::vector<Vec4f> generateTerrain(int dimensionLog2, Vec4f seed, float unitLength = 1.0f);
  static float sharpness;
private:
  static void doSquare(std::vector<float>& map, int mapWidth, int startIndex, int length, int passNumber);
  static void doDiamond(std::vector<float>& map, int mapWidth, int startIndex, int length, int passNumber);

  static void doSquarePass(std::vector<float>& map, int mapWidth, int length, int divisions);
  static void doDiamondPass(std::vector<float>& map, int mapWidth, int length, int divisions);

  static float getRandomOffset(int passNumber);
  static std::vector<Vec4f> createVectorMap(std::vector<float>& map, int mapWidth, float unitLength);

  static void showMap(std::vector<float>& map, int length);
};
