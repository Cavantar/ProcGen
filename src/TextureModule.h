#pragma once
#include <jpb\Noise.h>
#include "Includes.h"
#include "GLSLShader.h"
#include "TexturedQuad.h"
#include "ChunkMap.h"

class TextureModule {
public:

  // Creates Inital buffers and texture stuff
  // This implicitly uses the MapGenData thing
  void initialize(GLSLShader& shader, TwBar * const bar, const Vec2i& mainWindowSize);

  // Sets the internal controls
  void setTweakBar(TwBar * const bar);

  // Should be passed the screen space shader
  void render(GLSLShader& shader);
  void update(GLSLShader& shader, Vec2f& cameraPosition);
  void setMapGenData(const MapGenData* mapGenData) { this->mapGenData = mapGenData;}

private:
  const MapGenData* mapGenData;
  real32 aspectRatio;
  int32 chunkRadiusInTexture = 5;
  int32 prevChunkRadiusInTexture = chunkRadiusInTexture;

  Vec2f cameraChunkPosition = Vec2f();

  char filenameAnt[255];

  TexturedQuad texturedQuad;
  GLuint texBindingUnit = 1;

  GLuint texSampler;

  bool shouldSave = false;
  bool hideTexture = false;
  bool shouldRegenerate = false;

  int32 coordinateMultiplier = 50;
  int32 prevCoordinateMultiplier = coordinateMultiplier;

  void regenerateTexture(GLSLShader& shader);

  std::vector<real32> getMap(Vec2f offset, int32 sideLength, const std::unordered_map<int32,GenData>& genDataMap,
			     const std::string& expression, real32 baseWidthModifier = 1.0f);

  // Gets AppropriateColor from Color List.
  Vec3f getColor(real32 greyValue);
};
