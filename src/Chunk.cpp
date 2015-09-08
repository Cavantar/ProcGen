#include <unordered_map>
#include "Chunk.h"

void Chunk::joinThreadAndCopy(GLSLShader& shader)
{
  t.join();
  net.copyToGfx(shader);
  heightBounds = net.getBounds(2);
}

void Chunk::render(GLSLShader& shader, const RENDER_TYPE renderType, const GLuint globalMatricesUBO)
{
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

  // copying gendata

  this->genData = genData;
  this->sideLength = sideLength;
  this->expression = expression;

  t = std::thread(&Chunk::prepare, this);
}

void Chunk::prepare()
{
  Vec2u dimensions = Vec2u(sideLength, sideLength);

  std::vector<Vec4f>& map = Noise::getMapFast(Vec2f(position_x, position_y), sideLength, genData, expression, 1.0f, true);
  net.prepareDataWithBounds(dimensions, map);

  // vector<Vec4f>& map = Noise::getMapFast(Vec2f(position_x, position_y), sideLength, genData, expression, false);
  // net.prepareData(dimensions, map);

  ready = true;
}
