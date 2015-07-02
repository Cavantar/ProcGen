#pragma once 

#include "Includes.h"
#include "GLSLShader.h"

class Net{
public:
  Net(glm::uvec2& dimensions, vector<Vec4f>& vertices, GLSLShader& shader);
  ~Net();
  Net(){}
  
  // Though it might seem like grid can be non square I couldn't make it work with static maps
  // Containing glm::uvec2 as keys
  
  void prepareData(const glm::uvec2& dimensions, const vector<Vec4f>& vertices);
  void copyToGfx(GLSLShader& shader);
  void render(const RENDER_TYPE renderType) const;
  glm::vec2 getBounds(const int dimension) const;
  
private:
  vector<float> rawData;
  
  glm::ivec2 dimensions;
  
  int numbOfLines;
  int numbOfTriangles;
  
  vector<Vec4f> vertices;
  vector<glm::uvec2> lineIndexVec;
  vector<glm::uvec3> trianglesIndexVec;
  //vector<list<int>> adjacencyList;
  vector<Vec3f> normals;
  
  //OpenGL Objects;
  GLuint vao;
  GLuint bufferObject; // Contains all data
  GLuint triangleIndexBuffer;
  GLuint lineIndexBuffer;
  
  // If Stuff Was Generated Once There's no need to generate it again
  static map<int, vector<list<int>>> adjacencyLists;
  static map<int, vector<glm::uvec3>> trianglesIndexVecs;
  
  static mutex staticResourcesLock;
};
