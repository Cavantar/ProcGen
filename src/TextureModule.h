#pragma once
#include "Includes.h"
#include "GLSLShader.h"
#include "TexturedQuad.h"

class TextureModule {
public:

  // Creates Inital buffers and texture stuff
  void initialize(GLSLShader& shader, const Vec2i& mainWindowSize);

  // Sets the internal controls
  void setTweakBar(TwBar * const bar);

  // Should be passed the screen space shader
  void render(GLSLShader& shader);

private:
  real32 aspectRatio;
  TwBar* textureBar;

  TexturedQuad texturedQuad;
  GLuint texBindingUnit = 1;

  GLuint texSampler;
};
