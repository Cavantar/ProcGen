#include "GLSLShader.h"
#include <assert.h>

void
GLSLShader::loadFromString(GLenum whichShader, const std::string& source)
{
  GLuint shader = glCreateShader(whichShader);

  const char * ptmp = source.c_str();
  glShaderSource(shader, 1, &ptmp, NULL);

  GLint status = GL_TRUE;
  glCompileShader(shader);

  glCheckErrors();

  GL_CHECK_ERRORS;

  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

  if(status == GL_FALSE) {
    GLint infoLogLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
    GLchar * infoLog = new GLchar[infoLogLength];
    glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
    std::cerr << "Compile log: " << infoLog << std::endl;
    delete[] infoLog;
  }
  shaders[totalShaders++] = shader;
}

void
GLSLShader::createAndLinkProgram()
{
  program = glCreateProgram();

  if(shaders[ST_VERTEX] != 0) {
    glAttachShader(program, shaders[ST_VERTEX]);
    std::cout << "\tVertex\n";
  }
  if(shaders[ST_FRAGMENT] != 0) {
    glAttachShader(program, shaders[ST_FRAGMENT]);
    std::cout << "\tFragment\n";
  }
  if(shaders[ST_GEOMETRY] != 0) {
    glAttachShader(program, shaders[ST_GEOMETRY]);
    std::cout << "\tGeometry\n";
  }

  GLint status;
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &status);

  if(status == GL_FALSE) {
    GLint infoLogLength;

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
    GLchar* infoLog = new GLchar[infoLogLength];
    glGetProgramInfoLog(program, infoLogLength, NULL, infoLog);
    std::cerr << "Link log: " << infoLog << std::endl;
    delete[] infoLog;
    assert(0);
  }

  glDeleteShader(shaders[ST_VERTEX]);
  glDeleteShader(shaders[ST_FRAGMENT]);
  glDeleteShader(shaders[ST_GEOMETRY]);
}

void GLSLShader::use()
{
  glUseProgram(program);
}

void GLSLShader::unUse()
{
  glUseProgram(0);
}

void GLSLShader::addAttribute(const std::string& attribute)
{
  int temp = glGetAttribLocation(program, attribute.c_str());
  if(temp < 0) {
    std::cout << "glGetAttributeError\n";
    assert(0);
  }
  attributeList[attribute] = temp;
}

GLuint GLSLShader:: operator [](const std::string& attribute)
{
  return attributeList[attribute];
}

void GLSLShader::addUniform(const std::string& uniform)
{
  uniformLocationList[uniform] = glGetUniformLocation(program, uniform.c_str());
}

void GLSLShader::addUniformBlock(const std::string& uniformBlockName)
{
  uniformBlockIndex[uniformBlockName] = glGetUniformBlockIndex(program, uniformBlockName.c_str());
}

void GLSLShader::bindUniformBlock(const std::string& uniformBlockName, GLuint bindingIndex)
{
  glUniformBlockBinding(program, uniformBlockIndex[uniformBlockName], bindingIndex);
}

GLuint GLSLShader:: operator ()(const std::string& uniform) {
  return uniformLocationList[uniform];
}

void GLSLShader::loadFromFile(GLenum whichShader, const std::string& filename)
{
  std::ifstream fp;
  fp.open(filename.c_str(), std::ios::in);
  if(fp.is_open()) {
    std::string line, buffer;
    while(getline(fp, line)) {
      buffer.append(line);
      buffer.append("\r\n");
    }
    loadFromString(whichShader, buffer);
  }
  else {
    std::cerr << "Error loading shader: " << filename << std::endl;
  }
}

GLuint
GLSLShader::getUniform (const std::string& uniform) const
{
  return glGetUniformLocation(program, uniform.c_str());
}
