#pragma once
#include "Includes.h"
#include "GLSLShader.h"
#include "TexturedQuad.h"
#include <jpb\Noise.h>

struct ListColor{
  Vec3f color;
  real32 startValue;
  bool shouldDelete;
  int32 indexOnTheList;
};

typedef std::list<ListColor> ColorList;

class TextureModule {
public:

  // Creates Inital buffers and texture stuff
  void initialize(GLSLShader& shader, const Vec2i& mainWindowSize);

  // Sets the internal controls
  void setTweakBar(TwBar * const bar);

  // Should be passed the screen space shader
  void render(GLSLShader& shader);
  void update(GLSLShader& shader);

private:
  real32 aspectRatio;
  TwBar* textureBar;

  int32 textureResolutionX = 32;
  int32 prevTextureResolutionX = 32;
  TexturedQuad texturedQuad;
  GLuint texBindingUnit = 1;

  GLuint texSampler;

  // cString For Expression in Ant Tweak Bar
  char expressionAnt[255];
  char currentExpAnt[255];

  TwType noiseType;
  ColorList colorList;
  bool addColor = false;

  std::string currentExpression;
  std::string previousExpression;

  int currentMapIndex = 1;
  int prevMapIndex = 1;

  bool renderExpression = true;
  bool prevRenderExpression = true;

  bool hideTexture = false;

  GenDataMap genDataMap;
  bool shouldRegenerate = false;

  GenData genData, prevGenData;

  void regenerateTexture(GLSLShader& shader);

  std::vector<real32> getMap(Vec2f offset, int32 sideLength, std::list<GenData>& genDatas,
			     const std::string& expression);

  void addListColor(TwBar * const bar, ListColor listColor);
  void deleteListColor(TwBar * const bar, int32 colorIndex);
  void updateColors();

  // Gets AppropriateColor from Color List.
  Vec3f getColor(real32 greyValue);
};
