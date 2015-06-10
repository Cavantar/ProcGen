#pragma once 

#include "Includes.h"
#include "Net.h"
#include "Noise.h"


typedef std::list<GenData> GenDataList;

class Chunk{
public:
  atomic_bool ready;

  // Chunk Position NOT Absolute Position
  atomic_int position_x;
  atomic_int position_y;
  
  // Sqr(area)
  atomic_int sideLength;
  
  glm::vec2 heightBounds;
  GenDataList genData;
  
  //glm::ivec2 position; // Commented out because i won't be able to check if it's generating without it
  
  Chunk() { ready = false; }
  
  // After Thread Finishied We Have To Join Thread And Copy Data To Gfx
  void startPrepareThread(const glm::ivec2& position, const GenDataList& genData, const int sideLength);
  void joinThreadAndCopy(GLSLShader& shader);
  void render(GLSLShader& shader, const RENDER_TYPE renderType, const GLuint globalMatricesUBO);
private:
  Net net;
  thread t;
  
  void prepare();
};
