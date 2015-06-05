#pragma once 

#include "Window.h"
#include "GLSLShader.h"
#include "Net.h"
#include "Camera.h"
#include "TexturedQuad.h"
#include "ChunkMap.h"


class Game : public Window {
public:
  void setupAndStart();
  ~Game() { TwTerminate(); }
private:	
  glm::ivec2 mainWindowSize;
  //Gui - AntTweakBar
  TwBar* myBar;
  
  ChunkMap chunkMap;
  
  // Global Matrices Uniform Buffer Object and Binding Index
  GLuint globalMatricesUBO;
  GLuint globalMatricesUBI;
  
  // Texture Binding Unit
  GLuint texBindingUnit;
  
  GLSLShader shader, normalsShader, textureShader;
  
  glm::mat4 perspectiveMatrix;
  CameraPtr camera = CameraPtr(new FreeLookCamera());
  
  unsigned int debugCounter = 0;
  shared_ptr<Net> ground;
  
  TexturedQuad texturedQuad;
  TexturedQuad texturedQuad2;
  
  GLuint textureBufferObject;
  GLuint texDataBufferObject, texTriangleIndexBuffer, texVao;
  GLuint texSampler;
  
  void myRenderFunction();
  void render();
  void loadShaders();
  void setGlobalMatrices();
  void setTextureStuff();
  void setTexturedQuad();
  void setTweakBar();
};
