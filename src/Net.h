#pragma once

#include "Includes.h"
#include "GLSLShader.h"

// To use it first prepare data then copy it to gfx
// Afterwords it should work correctly

class Net{
public:
  Net(Vec2u& dimensions, std::vector<Vec4f>& vertices, GLSLShader& shader);
  ~Net();
  Net(){}

  // Though it might seem like grid can be non square I couldn't make it work with static maps
  // Containing Vec2u as keys

  void prepareData(const Vec2u& dimensions, const std::vector<Vec4f>& vertices);

  // We pass the dimensions of result vertices without bounds but used in calculations
  void prepareDataWithBounds(const Vec2u& internalDimensions, const std::vector<Vec4f>& vertices);
  void copyToGfx(GLSLShader& shader);
  void render(const RENDER_TYPE renderType) const;
  Vec2f getBounds(const int dimension) const;

  void saveToObj(std::string filename) const;

private:
  std::vector<float> rawData;

  Vec2u dimensions;

  int numbOfLines;
  int numbOfTriangles;

  std::vector<Vec4f> vertices;
  std::vector<Vec2u> lineIndexVec;
  std::vector<Vec3f> normals;

  //OpenGL Objects;
  GLuint vao;
  GLuint bufferObject; // Contains all data
  GLuint triangleIndexBuffer;
  GLuint lineIndexBuffer;

  // If Stuff Was Generated Once There's no need to generate it again
  static std::map<int, std::vector<std::list<int>>> adjacencyLists;
  static std::map<int, std::vector<Vec3u>> trianglesIndexVecs;

  //map<int, vector<Vec3u>> trianglesIndexVecs;

  static std::mutex staticResourcesLock;
};
