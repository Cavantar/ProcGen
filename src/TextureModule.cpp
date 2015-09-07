#include "TextureModule.h"
#include <jpb\Noise.h>

void
TextureModule::initialize(GLSLShader& shader, const Vec2i& mainWindowSize)
{
  aspectRatio = ((real32)mainWindowSize.x / mainWindowSize.y);
  textureBar = TwNewBar("Texture");
  TwDefine(" Texture label='TextureGen' position='1045 250' size='220 400' valueswidth=200 fontsize=1");

  setTweakBar(textureBar);

  vector<glm::vec3> textureData;
  int textureWidth = 128;
  int textureArea = textureWidth*textureWidth;
  textureData.resize(textureArea);
  float greyValue;

  float stepSize = 1.0f / textureWidth;

  Vec2f point00 = Vec2f(-0.5f, 0.5f);
  Vec2f point10 = Vec2f(0.5f, 0.5f);
  Vec2f point01 = Vec2f(-0.5f, -0.5f);
  Vec2f point11 = Vec2f(0.5f, -0.5f);

  NoiseParams noiseParams = { 2, 2, 2.0f, 0.5f, 1}; // { 2, 2, 2.0f, 0.5f};

  for(int y = 0; y < textureWidth; y++) {
    Vec2f point0 = Vec2f::lerp(point00, point01, ((float)y + 0.5f) * stepSize);
    Vec2f point1 = Vec2f::lerp(point10, point11, ((float)y + 0.5f) * stepSize);
    for(int x = 0; x < textureWidth; x++) {
      Vec2f point = Vec2f::lerp(point0, point1, ((float)x + 0.5f) * stepSize);

      greyValue = Noise::sumWorley(point, noiseParams);
      textureData[y * textureWidth + x] = glm::vec3(greyValue, greyValue, greyValue);
    }
  }

  texturedQuad.prepareData(textureData, textureWidth, 0.6, aspectRatio, Vec2f(0.8f, 0.65f));
  texturedQuad.copyToGfx(shader);
}

void
TextureModule::setTweakBar(TwBar * const bar)
{

}

void
TextureModule::render(GLSLShader& shader)
{
  shader.use();
  texturedQuad.render(RT_TRIANGLES, texBindingUnit);
  shader.unUse();
}
