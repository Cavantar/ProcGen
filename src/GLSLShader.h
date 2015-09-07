#pragma once

#include "Includes.h"

class GLSLShader{
public:
  GLSLShader() { shaders[0] = 0; shaders[1] = 0; shaders[2] = 0; }

  void loadFromString(GLenum whichShader, const string& source);
  void loadFromFile(GLenum whichShader, const string& filename);
  void createAndLinkProgram();
  void use();
  void unUse();
  void addAttribute(const string& attribute);
  void addUniform(const string& uniform);
  void addUniformBlock(const string& uniformBlockName);
  void bindUniformBlock(const string& uniformBlockName, GLuint bindingIndex);

  GLuint getUniformBlockIndex(const string& uniformBlockName) { return uniformBlockIndex[uniformBlockName]; }
  GLuint operator[] (const string& attribute);
  GLuint operator() (const string& uniform);
  void deleteShaderProgram();

private:
  enum ShaderType{ ST_VERTEX, ST_FRAGMENT, ST_GEOMETRY };
  GLuint program;
  int totalShaders = 0;
  GLuint shaders[3];
  map<string, GLuint> attributeList;
  map<string, GLuint> uniformLocationList;
  map<string, GLuint> uniformBlockIndex;
};
