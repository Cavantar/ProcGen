#pragma once

#include "Includes.h"

class TerrainGenerator{
public:
  static std::vector<glm::vec4> generateTerrain(int dimensionLog2, glm::vec4 seed, float unitLength = 1.0f);
  static float sharpness;
private:
  static void doSquare(std::vector<float>& map, int mapWidth, int startIndex, int length, int passNumber);
  static void doDiamond(std::vector<float>& map, int mapWidth, int startIndex, int length, int passNumber);

  static void doSquarePass(std::vector<float>& map, int mapWidth, int length, int divisions);
  static void doDiamondPass(std::vector<float>& map, int mapWidth, int length, int divisions);

  static float getRandomOffset(int passNumber);
  static std::vector<glm::vec4> createVectorMap(std::vector<float>& map, int mapWidth, float unitLength);

  static void showMap(std::vector<float>& map, int length);
};
