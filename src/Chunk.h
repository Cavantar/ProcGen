#pragma once

#include <jpb/Noise.h>
#include "Includes.h"
#include "Net.h"

typedef std::list<GenData> GenDataList;
// typedef std::map<int, GenData> GenDataMap;

class Chunk{
public:
  std::atomic_bool ready;

  // Chunk Position NOT Absolute Position
  std::atomic_int position_x;
  std::atomic_int position_y;

  // Sqr(area)
  std::atomic_int sideLength;

  Vec2f heightBounds;
  GenDataMap genDataMap;
  std::string expression;

  //glm::ivec2 position; // Commented out because i won't be able to check if it's generating without it

  Chunk() { ready = false; }

  // After Thread Finishied We Have To Join Thread And Copy Data To Gfx
  void startPrepareThread(const glm::ivec2& position, const GenDataMap& genDataMap, const int sideLength,
			  const std::string& expression);

  void joinThreadAndCopy(GLSLShader& shader);
  void render(GLSLShader& shader, const RENDER_TYPE renderType, const GLuint globalMatricesUBO);
  const Net& getNet() const { return net;}

private:
  Net net;
  std::thread t;

  void prepare();
};
