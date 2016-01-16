#include "TextureModule.h"
#include "Noise.h"
#include "SimpleParser.h"

void
TextureModule::initialize(GLSLShader& shader, TwBar * const bar, const Vec2i& mainWindowSize)
{
  aspectRatio = ((real32)mainWindowSize.x / mainWindowSize.y);
  // This implicitly uses the MapGenData thing
  regenerateTexture(shader);

  setTweakBar(bar);
}

void
TextureModule::setTweakBar(TwBar * const bar)
{

  TwAddVarRW(bar, "FileName", TW_TYPE_CSSTRING(sizeof(filenameAnt)), filenameAnt, "group='SavingTexture'");
  TwAddVarRW(bar, "SaveTexture", TW_TYPE_BOOLCPP, &shouldSave,
	     " label='SaveFile' group='SavingTexture'");

  TwDefine(" MapGen/'SavingTexture' group='TextureGen'");

  TwAddVarRW(bar, "RegenerateTex", TW_TYPE_BOOLCPP, &shouldRegenerate,
	     " label='RegenerateTex' group='TextureGen'");

  TwAddVarRW(bar, "Hide Texture", TW_TYPE_BOOLCPP, &hideTexture,
	     " label='Hide Texture' group='TextureGen'");

  TwAddVarRW(bar, "Chunk Radius", TW_TYPE_INT32, &chunkRadiusInTexture,
	     " label='Chunk Radius' group='TextureGen'");

  TwAddVarRW(bar, "Coordinate Multipler", TW_TYPE_INT32, &coordinateMultiplier,
	     " label='Coordinate Multipler' group='TextureGen'");

  TwDefine(" MapGen/'TextureGen' opened=false ");
}

void
TextureModule::update(GLSLShader& shader, Vec2f& cameraPosition)
{
  cameraChunkPosition = Vec2f((int)floor((cameraPosition.x - 50.0f) / 100) + 1 ,
			      -(int)floor((cameraPosition.y + 50) / 100));

  // if settings changed we update genData in genDataMap and regenerate chunks which
  if(mapGenData->shouldRegenerate || mapGenData->colorChanged ||
     coordinateMultiplier != prevCoordinateMultiplier ||
     chunkRadiusInTexture != prevChunkRadiusInTexture)
  {
    shouldRegenerate = true;
  }

  prevCoordinateMultiplier = coordinateMultiplier;
  prevChunkRadiusInTexture = chunkRadiusInTexture;

  if(shouldSave)
  {
    std::string filename = "textures/";
    filename += filenameAnt;
    filename += ".bmp";

    std::cout << "Saving texture to file: " << filename << std::endl;

    int32 textureResolutionX = mapGenData->resolution;
    saveTexture(texturedQuad.getTextureData(), Vec2u(textureResolutionX, textureResolutionX), filename);

    shouldSave = false;
  }

  // Regenerating chunks if it's required
  if(shouldRegenerate)
  {
    regenerateTexture(shader);
    shouldRegenerate = false;
  }
}

void
TextureModule::render(GLSLShader& shader)
{
  if(!hideTexture)
  {
    shader.use();
    texturedQuad.render(RT_TRIANGLES, texBindingUnit);
    shader.unUse();
  }
}

void
TextureModule::regenerateTexture(GLSLShader& shader)
{
  std::vector<Vec3f> textureData;
  int32 textureResolutionX = mapGenData->resolution;
  int textureWidth = textureResolutionX;
  int textureArea = textureWidth*textureWidth;
  textureData.resize(textureArea);

  std::string expression = mapGenData->currentExpression;
  if(!mapGenData->renderExpression)
    expression = "Map" + std::to_string(mapGenData->currentMapIndex);

  GenDataMap filteredGenDataMap = mapGenData->getFilteredGenDataMap(expression);
  std::vector<real32> heightValues = getMap(cameraChunkPosition, textureResolutionX, filteredGenDataMap, expression,
					    (1 + (chunkRadiusInTexture - 1) * 2));

  for(int i = 0; i != textureData.size(); i++)
  {
    real32 greyValue = heightValues[i];
    Vec3f colorValue = getColor(greyValue);

    textureData[i] = colorValue;
  }

  // TexturedQuad newQuad;
  texturedQuad.cleanUp();
  texturedQuad.prepareData(textureData, textureWidth, 1.0, aspectRatio, Vec2f(0.65f, 0.35f));
  texturedQuad.copyToGfx(shader);
  // texturedQuad = newQuad;
}

Vec3f
TextureModule::getColor(real32 greyValue)
{
  Vec3f resultColor = Vec3f(greyValue, greyValue, greyValue);

  const ColorList& colorList = mapGenData->colorList;

  auto bottomColorIt = colorList.begin();
  auto topColorIt = colorList.begin();
  topColorIt++;

  for(auto it = topColorIt; it != colorList.end(); it++)
  {
    topColorIt = it;

    ListColor bottomColor = *bottomColorIt;
    ListColor topColor = *topColorIt;

    if(greyValue >= bottomColor.startValue &&
       greyValue <= topColor.startValue)
    {
      real32 valueDelta = (topColor.startValue - bottomColor.startValue);
      real32 tValue = (greyValue - bottomColor.startValue) / valueDelta;

      resultColor = Vec3f::lerp(bottomColor.color, topColor.color, tValue);
      break;
    }

    bottomColorIt = topColorIt;
  }

  return resultColor;
}

std::vector<real32>
TextureModule::getMap(Vec2f offset, int32 sideLength, const std::unordered_map<int32,GenData>& genDataMap,
		      const std::string& expression, real32 baseWidthModifier)
{
  int32 numbOfVertices = -1;
  numbOfVertices = sideLength * sideLength;

  std::vector<real32> result;
  result.resize(numbOfVertices);

  real32 stepSize = 1.0f / (sideLength - 1);
  real32 greyValue;

  Vec2f point00 = Vec2f(-0.5f, 0.5f) * baseWidthModifier;
  Vec2f point10 = Vec2f(0.5f, 0.5f) * baseWidthModifier;
  Vec2f point01 = Vec2f(-0.5f, -0.5f) * baseWidthModifier;
  Vec2f point11 = Vec2f(0.5f, -0.5f) * baseWidthModifier;

  // Parser Stuff ----------------
  SimpleParser simpleParser;
  VariableMap variableMap;
  // I set variable Map first becauuuse ???
  simpleParser.setVariableMap(&variableMap);
  EntryList reversePolish = simpleParser.getReversePolish(expression);
  // -----------------------------
  std::map<uint32, Vec4f> values;
  std::vector<VariableMap> variableMapBuffer;

  std::unordered_map<int32, real32> currentPointMapValues;
  Vec2f currentPointRealPosition;

  // Setting pointer to variables in parser - because map insertion is too expensive
  // to be done every sampled point
  // Increased efficiency by the factor of 2
  for(auto it = genDataMap.begin(); it != genDataMap.end(); it++)
  {
    int32 mapIndex = it->first;
    std::string mapName = "Map" + std::to_string(mapIndex);
    variableMap[mapName] = &currentPointMapValues[mapIndex];
  }

  variableMap["x"] = &currentPointRealPosition.x;
  variableMap["y"] = &currentPointRealPosition.y;

  // TODO: Make Value Noise More Efficient
  for(int32 valIndex = 0; valIndex < numbOfVertices; valIndex += 4)
  {
    Vec2f points[4];
    Vec2f realPositions[4];

    for(int i = 0; i < 4; i++)
    {
      int32 currValIndex = valIndex + i;

      int32 x = currValIndex % sideLength;
      int32 y = currValIndex / sideLength;

      Vec2f point0 = Vec2f::lerp(point00, point01, ((float)y ) * stepSize);
      Vec2f point1 = Vec2f::lerp(point10, point11, ((float)y ) * stepSize);

      points[i] = Vec2f::lerp(point0, point1, ((float)x) * stepSize);
      realPositions[i] = points[i] + offset;
    }

    for(auto it = genDataMap.begin(); it != genDataMap.end(); it++) {
      int index = it->first;
      // std::cout << index << std::endl;
      const GenData& genData = it->second;
      if(genData.noiseType == NT_PERLIN)
      {

	values[index] = Noise::sumPerlinFast(realPositions, genData.noiseParams);

	if(genData.noiseParams.extraParam == 0)
	{
	  values[index] *= 0.5f;
	  values[index] += 0.5f;
	}
	values[index] *= genData.scale;
      }
      else if(genData.noiseType == NT_VALUE)
      {
	for(int i = 0; i < 4; i++)
	{
	  values[index][i] = Noise::sumValue(realPositions[i], genData.noiseParams);
	}
	values[index] *= genData.scale;
      }
      else if(genData.noiseType == NT_WORLEY)
      {
	for(int i = 0; i < 4; i++)
	{
	  values[index][i] = Noise::sumWorley(realPositions[i], genData.noiseParams);
	}
	values[index] *= genData.scale;
      }
      else std::cout << "Error in Noise getMapFast No such noise type. \n";
    }

    for(int i = 0; i < 4; i++)
    {
      int32 currValIndex = valIndex + i;

      int32 x = currValIndex % sideLength;
      int32 y = currValIndex / sideLength;

      real32 finalValue = 0;

      for(auto it = values.begin(); it != values.end(); it++)
      {
	int32 index = it->first;
	currentPointMapValues[index] = values[index][i];
      }
      currentPointRealPosition = realPositions[i] * 50;

      variableMap["x"] = &realPositions[i].x;
      variableMap["y"] = &realPositions[i].y;

      EntryList reversePolishCopy = reversePolish;
      finalValue = simpleParser.evaluateExpression(reversePolishCopy);

      // Fucking fix this bullshit

      if(currValIndex < numbOfVertices)
	result[currValIndex] = finalValue;
      else break;
    }
  }

  return result;
}
