#pragma once

#include "Includes.h"
#include "GLSLShader.h"

class TexturedQuad{
public:
  ~TexturedQuad();
  void prepareData(const vector<Vec3f>& textureData, const int textureWidth, const float objectWidth,
		   const real32 aspectRation = 1.0f, Vec2f offset = Vec2f());

  void copyToGfx(GLSLShader& shader);

  void render(const RENDER_TYPE renderType, const GLuint texBindingUnit) const;

  void cleanUp();

private:
  vector<Vec3f> textureData;
  int textureWidth;

  vector<Vec4f> vertices;
  vector<Vec2f> texCoords;
  vector<Vec3u> triangleIndexVec;

  vector<float> rawData;

  GLuint textureDataBufferObject;
  GLuint dataBufferObject, triangleIndexBuffer, vao;

};
