#pragma once

#include <jpb/Noise.h>
#include "Includes.h"
#include "Net.h"

typedef std::list<GenData> GenDataList;

class Chunk{
public:
  std::atomic_bool ready;

  // Chunk Position NOT Absolute Position
  Vec2i position;

  // Sqr(area)
  int32 sideLength;
  Vec2f heightBounds;
  GenDataMap genDataMap;
  std::string expression;

  Chunk() { ready = false; }

  // After Thread Finishied We Have To Join Thread And Copy Data To Gfx
  void startPrepareThread(const Vec2i& position, const GenDataMap& genDataMap, const int sideLength,
			  const std::string& expression);

  void joinThreadAndCopy(GLSLShader& shader);
  void render(GLSLShader& shader, const RENDER_TYPE renderType, const GLuint globalMatricesUBO);

  const Net& getNet() const { return net;}
  static void erode(std::vector<Vec4f>& map, const Vec2u& dimensions);

private:
  Net net;
  std::thread t;

  void prepare();
};
