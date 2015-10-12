#include <unordered_map>
#include "Chunk.h"

void
Chunk::joinThreadAndCopy(GLSLShader& shader)
{
  t.join();
  net.copyToGfx(shader);
  heightBounds = net.getBounds(2);
}

void
Chunk::render(GLSLShader& shader, const RENDER_TYPE renderType, const GLuint globalMatricesUBO)
{
  glBindBuffer(GL_UNIFORM_BUFFER, globalMatricesUBO);

  Mat4 translationMatrix = Mat4::createTranslationMatrix(Vec3f(position_x * 100, 0, -position_y * 100));
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Mat4) * 2, sizeof(Mat4), &translationMatrix);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  net.render(renderType);
}

void
Chunk::startPrepareThread(const Vec2i& position, const GenDataMap& genDataMap, const int sideLength,
			       const std::string& expression) {
  position_x = position.x;
  position_y = position.y;

  // copying gendata

  this->genDataMap = genDataMap;
  this->sideLength = sideLength;
  this->expression = expression;

  t = std::thread(&Chunk::prepare, this);
}

void
Chunk::prepare()
{
  Vec2u dimensions = Vec2u(sideLength, sideLength);

  std::vector<Vec4f>& map = Noise::getMapFast(Vec2f(position_x, position_y), sideLength, genDataMap, expression, 1.0f, true);
  // erode(map, dimensions);
  net.prepareDataWithBounds(dimensions, map);

  // vector<Vec4f>& map = Noise::getMapFast(Vec2f(position_x, position_y), sideLength, genData, expression, false);
  // net.prepareData(dimensions, map);

  ready = true;
}

void
Chunk::erode(std::vector<Vec4f>& map, const Vec2u& dimensions)
{
  real32 diffThreshold = 1.0f;
  int iterations = 1;
  for(int i = 0; i < iterations; i++)
  {
    for(int y = 2; y < dimensions.y; y++)
    {
      int currentRowIndex = y * (dimensions.x+2);
      int upRowIndex = (y+1) * (dimensions.x+2);
      int downRowIndex = (y-1) * (dimensions.x+2);

      for(int x = 2; x < dimensions.x; x++)
      {
	int currentIndex = currentRowIndex + x;
	int leftIndex = currentIndex - 1;
	int rightIndex = currentIndex + 1;
	int upIndex = upRowIndex + x;
	int downIndex = downRowIndex + x;

	real32 maxDiff = 0;
	int maxIndex = -1;

	real32 tempDiff = map[currentIndex].y - map[leftIndex].y;
	if(tempDiff > diffThreshold && tempDiff > maxDiff)
	{
	  maxDiff = tempDiff;
	  maxIndex = leftIndex;
	}

	tempDiff = map[currentIndex].y - map[rightIndex].y;
	if(tempDiff > diffThreshold && tempDiff > maxDiff)
	{
	  maxDiff = tempDiff;
	  maxIndex = rightIndex;
	}

	tempDiff = map[currentIndex].y - map[upIndex].y;
	if(tempDiff > diffThreshold && tempDiff > maxDiff)
	{
	  maxDiff = tempDiff;
	  maxIndex = upIndex;
	}

	tempDiff = map[currentIndex].y - map[downIndex].y;
	if(tempDiff > diffThreshold && tempDiff > maxDiff)
	{
	  maxDiff = tempDiff;
	  maxIndex = downIndex;
	}

	if(maxIndex != -1)
	{
	  real32 delta = maxDiff / 2.0f;
	  map[currentIndex].y -= delta;
	  // map[maxIndex].y += delta;
	}
      }
    }
  }
}
