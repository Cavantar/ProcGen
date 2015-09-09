#pragma once

#include "Window.h"
#include "GLSLShader.h"
#include "Net.h"
#include "Camera.h"
#include "TexturedQuad.h"
#include "ChunkMap.h"
#include "TextureModule.h"

class Game : public Window {
public:
  void setupAndStart();
  ~Game() { TwTerminate(); }
private:
  glm::ivec2 mainWindowSize;
  //Gui - AntTweakBar

  TwBar* terrainBar;
  TwBar* mapGenBar;


  MapGenData mapGenData;
  ChunkMap chunkMap;
  TextureModule textureModule;

  // Global Matrices Uniform Buffer Object and Binding Index
  GLuint globalMatricesUBO;
  GLuint globalMatricesUBI;

  // Texture Binding Unit
  GLuint texBindingUnit;

  GLSLShader shader, normalsShader, textureShader;
  GLSLShader ssTextureShader;

  glm::mat4 perspectiveMatrix;
  CameraPtr camera = CameraPtr(new FreeLookCamera());

  unsigned int debugCounter = 0;
  std::shared_ptr<Net> ground;

  TexturedQuad texturedQuad;
  TexturedQuad texturedQuad2;

  GLuint textureBufferObject;
  GLuint texDataBufferObject, texTriangleIndexBuffer, texVao;
  GLuint texSampler;

  Net testNet;
  Net testNet2;
  Net testNet3;

  void myRenderFunction();
  void render();
  void loadShaders();
  void setGlobalMatrices();
  void setTextureStuff();
  void setTexturedQuad();
  void setTweakBar();
};
