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
  Vec2i mainWindowSize;
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

  Mat4 perspectiveMatrix;
  CameraPtr camera = CameraPtr(new FreeLookCamera());

  unsigned int debugCounter = 0;

  TexturedQuad texturedQuad;

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
