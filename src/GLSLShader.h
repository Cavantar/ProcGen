#pragma once

#include "Includes.h"

class GLSLShader{
public:
  GLSLShader() { shaders[0] = 0; shaders[1] = 0; shaders[2] = 0; }

  void loadFromString(GLenum whichShader, const std::string& source);
  void loadFromFile(GLenum whichShader, const std::string& filename);
  void createAndLinkProgram();
  void use();
  void unUse();
  void addAttribute(const std::string& attribute);
  void addUniform(const std::string& uniform);
  void addUniformBlock(const std::string& uniformBlockName);
  void bindUniformBlock(const std::string& uniformBlockName, GLuint bindingIndex);

  GLuint getUniformBlockIndex(const std::string& uniformBlockName) { return uniformBlockIndex[uniformBlockName]; }
  GLuint operator[] (const std::string& attribute);
  GLuint operator() (const std::string& uniform);
  GLuint getUniform (const std::string& uniform) const;
  void deleteShaderProgram();

private:
  enum ShaderType{ ST_VERTEX, ST_FRAGMENT, ST_GEOMETRY };
  GLuint program;
  int totalShaders = 0;
  GLuint shaders[3];

  std::map<std::string, GLuint> attributeList;
  std::map<std::string, GLuint> uniformLocationList;
  std::map<std::string, GLuint> uniformBlockIndex;
};
