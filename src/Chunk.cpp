#include "Chunk.h"
#include "Noise.h"


void Chunk::joinThreadAndCopy(GLSLShader& shader) {
  t.join();
  net.copyToGfx(shader);
  heightBounds = net.getBounds(2);
}
void Chunk::render(GLSLShader& shader, const RENDER_TYPE renderType, const GLuint globalMatricesUBO) {
  glBindBuffer(GL_UNIFORM_BUFFER, globalMatricesUBO);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, sizeof(glm::mat4),
		  glm::value_ptr(glm::translate(glm::mat4(), glm::vec3(position_x * 100, 0, -position_y * 100))));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  
  //glUniform2fv(shader("heightBounds"), 1, &heightBounds.x);
  
  net.render(renderType);
}
void Chunk::startPrepareThread(const glm::ivec2& position, const GenData& genData, const int sideLength) {
  position_x = position.x;
  position_y = position.y;
  
  this->genData = genData;
  this->sideLength = sideLength;
  t = thread(&Chunk::prepare, this);
}
void Chunk::prepare() {
  
  glm::uvec2 dimensions = glm::uvec2(sideLength, sideLength);
  
  list<GenData> genDatas;
  
  // The Base
  GenData elevation = {NT_PERLIN, {0.2f, 5, 2.0f, 0.4f}, 2.0f};
  GenData roughness = { NT_PERLIN, { 0.3f, 3, 2.5f, 0.4f }, 2.0f };
  GenData detail = genData;
  
  //detail = { NT_PERLIN, { 0.4f, 6, 2.5f, 0.3f }, 2.0f };
  
#if 1
  genDatas.push_back(elevation);
  genDatas.push_back(roughness);
#endif
  
  genDatas.push_back(detail);
  
  net.prepareData(dimensions, Noise::getMap(glm::vec2(position_x, position_y), sideLength, genDatas));
  ready = true;
}
