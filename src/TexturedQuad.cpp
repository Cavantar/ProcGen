#include "TexturedQuad.h"

TexturedQuad::~TexturedQuad()
{
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &dataBufferObject);
  glDeleteBuffers(1, &triangleIndexBuffer);
  glDeleteBuffers(1, &textureDataBufferObject);
}

void
TexturedQuad::prepareData(const vector<glm::vec3>& textureData,const int textureWidth, const float objectWidth,
			  const real32 aspectRatio, Vec2f offset)
{
  // Constructing Texture Data

  this->textureData = textureData;
  this->textureWidth = textureWidth;

  // Vertices

  vertices.resize(4);

  float width;
  float height;

  // To make it half on either side
  height = objectWidth / 2.0f;
  //  height = width * (1.0f/aspectRatio);
  width = height * (1.0f / aspectRatio);

  // offset *= Vec2f(aspectRatio, 1.0f);

  // std::cout << "Aspect ratio: " << aspectRatio << std::endl;
  vertices[0] = glm::vec4(-width + offset.x, -height + offset.y, 0, 1.0f);
  vertices[1] = glm::vec4(width + offset.x, -height + offset.y, 0, 1.0f);
  vertices[2] = glm::vec4(width + offset.x, height + offset.y, 0, 1.0f);
  vertices[3] = glm::vec4(-width + offset.x, height + offset.y, 0, 1.0f);

  // TexCoords

  texCoords.resize(4);

  texCoords[0] = glm::vec2(0, 1.0f);
  texCoords[1] = glm::vec2(1.0f, 1.0f);
  texCoords[2] = glm::vec2(1.0f, 0);
  texCoords[3] = glm::vec2(0, 0);

  // Preparing Data Buffer

  rawData.resize(vertices.size() * sizeof(glm::vec4) + texCoords.size() *sizeof(glm::vec2));
  // Copying vertices
  memcpy(&rawData[0], &vertices[0], vertices.size() * sizeof(glm::vec4));
  // Copying texCoords
  memcpy(&rawData[vertices.size() * 4], &texCoords[0], texCoords.size() * sizeof(glm::vec2));

  // TriangleIndexBuffer
  triangleIndexVec.resize(2);

  triangleIndexVec[0] = glm::uvec3(0, 2, 1);
  triangleIndexVec[1] = glm::uvec3(0, 3, 2);
}

void
TexturedQuad::copyToGfx(GLSLShader& shader)
{
  // OpenGL Stuff

  glGenTextures(1, &textureDataBufferObject);
  glBindTexture(GL_TEXTURE_2D, textureDataBufferObject);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureWidth, 0, GL_RGB, GL_FLOAT, &textureData[0]);

  // Linear filtering
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Copying To Gfx
  // GLuint texBufferObject, triangleIndexBuffer, texVao;

  glGenBuffers(1, &triangleIndexBuffer);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleIndexVec.size() * sizeof(glm::uvec3), &triangleIndexVec[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glGenBuffers(1, &dataBufferObject);

  glBindBuffer(GL_ARRAY_BUFFER, dataBufferObject);
  glBufferData(GL_ARRAY_BUFFER, rawData.size() * 4, &rawData[0], GL_STREAM_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Tworzenie VAO

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, dataBufferObject);

  glEnableVertexAttribArray(shader["position"]);
  glVertexAttribPointer(shader["position"], 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(shader["texCoord"]);
  glVertexAttribPointer(shader["texCoord"], 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(vertices.size() * sizeof(glm::vec4)));

  glBindVertexArray(0);

  GL_CHECK_ERRORS;
}

void
TexturedQuad::render(const RENDER_TYPE renderType, const GLuint texBindingUnit) const
{
  glActiveTexture(GL_TEXTURE0 + texBindingUnit);
  glBindTexture(GL_TEXTURE_2D, textureDataBufferObject);

  glBindVertexArray(vao);
  switch(renderType) {
  case RT_POINTS:
    glDrawArrays(GL_POINTS, 0, 4);
    break;
  default:
    glDrawArrays(GL_POINTS, 0, 4);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexBuffer);
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, 0);
    break;
  }
  glBindVertexArray(0);

}
