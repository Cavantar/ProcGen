#pragma once
#include "Includes.h"
#include "GLSLShader.h"
#include "TexturedQuad.h"
#include "ChunkMap.h"
#include <jpb\Noise.h>

class TextureModule {
public:

  // Creates Inital buffers and texture stuff
  // This implicitly uses the MapGenData thing
  void initialize(GLSLShader& shader, TwBar * const bar, const Vec2i& mainWindowSize);

  // Sets the internal controls
  void setTweakBar(TwBar * const bar);

  // Should be passed the screen space shader
  void render(GLSLShader& shader);
  void update(GLSLShader& shader);
  void setMapGenData(const MapGenData* mapGenData) { this->mapGenData = mapGenData;}

private:
  const MapGenData* mapGenData;
  real32 aspectRatio;

  char filenameAnt[255];

  TexturedQuad texturedQuad;
  GLuint texBindingUnit = 1;

  GLuint texSampler;

  bool shouldSave = false;
  bool hideTexture = false;
  bool shouldRegenerate = false;

  void regenerateTexture(GLSLShader& shader);

  std::vector<real32> getMap(Vec2f offset, int32 sideLength, std::list<GenData>& genDatas,
			     const std::string& expression);

  // Gets AppropriateColor from Color List.
  Vec3f getColor(real32 greyValue);
};
