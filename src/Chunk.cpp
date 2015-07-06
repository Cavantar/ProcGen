#include "Chunk.h"

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

void Chunk::startPrepareThread(const glm::ivec2& position, const GenDataList& genData, const int sideLength,
			       const std::string& expression) {
  position_x = position.x;
  position_y = position.y;
  
  this->genData = genData;
  this->sideLength = sideLength;
  this->expression = expression;
  
  t = thread(&Chunk::prepare, this);
}

void Chunk::prepare() {
  glm::uvec2 dimensions = glm::uvec2(sideLength, sideLength);
  
  vector<Vec4f>& map = Noise::getMapFast(Vec2f(position_x, position_y), sideLength, genData, expression);
  net.prepareData(dimensions, map);
  ready = true;
}
