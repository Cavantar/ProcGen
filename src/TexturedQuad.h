#pragma once

#include "Includes.h"
#include "GLSLShader.h"

class TexturedQuad{
public:
  ~TexturedQuad();
  void prepareData(const std::vector<Vec3f>& textureData, const int textureWidth, const float objectWidth,
		   const real32 aspectRation = 1.0f, Vec2f offset = Vec2f());

  void copyToGfx(GLSLShader& shader);

  void render(const RENDER_TYPE renderType, const GLuint texBindingUnit) const;
  std::vector<Vec3f>& getTextureData() { return textureData; };

  void cleanUp();

private:
  std::vector<Vec3f> textureData;
  int textureWidth;

  std::vector<Vec4f> vertices;
  std::vector<Vec2f> texCoords;
  std::vector<Vec3u> triangleIndexVec;

  std::vector<float> rawData;

  GLuint textureDataBufferObject;
  GLuint dataBufferObject, triangleIndexBuffer, vao;

};
