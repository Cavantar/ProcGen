#include "TextureModule.h"
#include <jpb\Noise.h>
#include <jpb\SimpleParser.h>

void
TextureModule::initialize(GLSLShader& shader, const Vec2i& mainWindowSize)
{
  aspectRatio = ((real32)mainWindowSize.x / mainWindowSize.y);
  textureBar = TwNewBar("Texture");
  TwDefine(" Texture label='TextureGen' position='1045 250' size='220 400' valueswidth=100 fontsize=1");

  setTweakBar(textureBar);

  genData = { NT_PERLIN, { 8.0f, 8, 2.0f, 0.4f }, 1.0f } ;
  genDataMap[1] = genData;
  prevGenData = genData;

  genDataMap[2] = { NT_PERLIN, { 8.0f, 8, 2.0f, 0.37f}, 1.0f };
  genDataMap[3] = { NT_WORLEY, { 8.0f, 1, 2.5f, 0.4f }, 1.0f };

  currentExpression = "Map1";
  std::cout << "Texture Expression: " << currentExpression << std::endl;

  previousExpression = currentExpression;
  sprintf(expressionAnt, currentExpression.c_str());
  sprintf(currentExpAnt, currentExpression.c_str());

  addListColor(textureBar, {Vec3f(), 0, false});
  addListColor(textureBar, {Vec3f(1.0f, 1.0f, 1.0f), 1.0, false});
  regenerateTexture(shader);

}

void
TextureModule::setTweakBar(TwBar * const bar)
{
  TwEnumVal noiseTypeEV[] = {{NT_PERLIN, "Perlin"}, {NT_VALUE, "Value"}, {NT_WORLEY, "Worley"}};
  noiseType = TwDefineEnum("NoiseType___", noiseTypeEV, 3);

  TwAddVarRW(bar, "Pattern", TW_TYPE_CSSTRING(sizeof(expressionAnt)), expressionAnt, "group='Generation'");

  TwAddVarRW(bar, "MapIndex: ", TW_TYPE_INT32, &currentMapIndex,
	     " label='MapIndex' min=1 max=10 step=1 keyIncr='+' keyDecr='-' group='Generation'");

  TwAddVarRW(bar, "RenderExpression", TW_TYPE_BOOLCPP, &renderExpression,
	     " label='RenderExpression' group='Generation'");

  TwAddVarRW(bar, "CurrentExpression", TW_TYPE_CSSTRING(sizeof(currentExpAnt)), currentExpAnt, "group='Generation' readonly=true");

  TwAddVarRW(bar, "NoiseType", noiseType, &genData.noiseType, "group='Noise Parameters'");

  TwAddVarRW(bar, "Frequency", TW_TYPE_FLOAT, &genData.noiseParams.frequency,
	     " label='Frequency' min=-0.050 max=100 step=0.025 keyIncr='+' keyDecr='-' group='Noise Parameters'");
  TwAddVarRW(bar, "Octaves", TW_TYPE_INT32, &genData.noiseParams.octaves,
	     " label='Octaves' min=1 max=10 step=1 keyIncr='+' keyDecr='-' group='Noise Parameters'");
  TwAddVarRW(bar, "Lacunarity", TW_TYPE_FLOAT, &genData.noiseParams.lacunarity,
	     " label='Lacunarity' min=1.1 max=10.0 step=0.05 keyIncr='+' keyDecr='-' group='Noise Parameters'");
  TwAddVarRW(bar, "Persistence", TW_TYPE_FLOAT, &genData.noiseParams.persistence,
	     " label='Persistence' min=0.05 max=1.0 step=0.05 keyIncr='+' keyDecr='-' group='Noise Parameters'");
  TwAddVarRW(bar, "Scale", TW_TYPE_FLOAT, &genData.scale,
	     " label='Scale' min=0.1 max=50.0 step=0.1 keyIncr='+' keyDecr='-'  group='Noise Parameters'");
  TwAddVarRW(bar, "extraParam", TW_TYPE_INT32, &genData.noiseParams.extraParam,
	     " label='extraParam' min=0 max=10 step=1 keyIncr='+' keyDecr='-' group='Noise Parameters'");
  TwAddVarRW(bar, "extraParam2", TW_TYPE_INT32, &genData.noiseParams.extraParam2,
	     " label='extraParam2' min=0 max=10 step=1 keyIncr='+' keyDecr='-' group='Noise Parameters'");

  TwDefine(" Texture/'Noise Parameters' group='Generation' ");

  TwAddVarRW(bar, "Regenerate", TW_TYPE_BOOLCPP, &shouldRegenerate,
	     " label='Regenerate' group='Generation'");

  TwAddVarRW(bar, "Resolution", TW_TYPE_INT32, &textureResolutionX,
	     " label='Resolution' min=16 max=256 step=16 keyIncr='+' keyDecr='-' group='TextureProperties'");

  TwAddVarRW(bar, "Hide Texture", TW_TYPE_BOOLCPP, &hideTexture,
	     " label='Hide Texture' group='TextureProperties'");

  TwAddVarRW(bar, "AddColor", TW_TYPE_BOOLCPP, &addColor,
	     " label='AddColor' group='Colors'");

  TwAddSeparator(bar, NULL, " group='Colors' ");
}

void
TextureModule::addListColor(TwBar * const bar, ListColor newListColor)
{
  static int maxIndex = 1;

  newListColor.indexOnTheList = maxIndex;
  colorList.push_back(newListColor);
  ListColor& listColor = colorList.back();
  std::string colorName = ("Color: " + std::to_string(maxIndex));

  std::string colorProperties = " label='" + colorName + "'  group='Colors'";

  TwAddVarRW(bar, colorName.c_str(), TW_TYPE_COLOR3F, &listColor.color,
	     colorProperties.c_str());

  TwAddVarRW(bar, ("StartValue: " + std::to_string(maxIndex)).c_str(), TW_TYPE_FLOAT, &listColor.startValue,
	     " label='StartValue' min=0 max=10 step=0.1 keyIncr='+' keyDecr='-' group='Colors'");

  TwAddVarRW(bar, ("DeleteColor: " + std::to_string(maxIndex)).c_str(), TW_TYPE_BOOLCPP, &listColor.shouldDelete,
	     " label='DeleteColor' group='Colors'");

  TwAddSeparator(bar, ("ColorSep: " + std::to_string(maxIndex)).c_str(), " group='Colors' ");

  ++maxIndex;
}

void
TextureModule::deleteListColor(TwBar * const bar, int32 colorIndex)
{
  TwRemoveVar(bar, ("Color: " + std::to_string(colorIndex)).c_str());
  TwRemoveVar(bar, ("StartValue: " + std::to_string(colorIndex)).c_str());
  TwRemoveVar(bar, ("DeleteColor: " + std::to_string(colorIndex)).c_str());
  TwRemoveVar(bar, ("ColorSep: " + std::to_string(colorIndex)).c_str());
}

void
TextureModule::update(GLSLShader& shader)
{
  static GenData defaultGenData = { NT_PERLIN, { 0.75f, 5, 2.0f, 0.4f }, 2.0f } ;

  // If selected mapIndex changed
  if(prevMapIndex != currentMapIndex)
  {
    // And the current genData specified by index doesn't exist create it and copy the default one
    if(genDataMap.count(currentMapIndex) == 0)
    {
      genDataMap[currentMapIndex] = defaultGenData;
    }

    // Setting current genData as specified by index (cause its changed)
    genData = genDataMap[currentMapIndex];

    // If were not rendering expression and map index changed we have to regenerate map
    if(!renderExpression) shouldRegenerate = true;
  }
  prevMapIndex = currentMapIndex;

  // if settings changed we update genData in genDataMap and regenerate chunks which
  if(genData != genDataMap[currentMapIndex] || textureResolutionX != prevTextureResolutionX)
  {
    genDataMap[currentMapIndex] = genData;
    shouldRegenerate = true;
  }
  prevTextureResolutionX = textureResolutionX;

  previousExpression = currentExpression;
  if(currentExpression != expressionAnt)
    // Checking if expression is valid
  {
    static std::string lastInvalidExp = "";
    if(lastInvalidExp != expressionAnt)
    {
      // static std::string lastInvalid = "";
      std::list<std::string> validVariables{"x", "y"};

      for(int32 j = 0; j < genDataMap.size(); j++)
      {
	std::string mapName = "Map" + std::to_string(j+1);
	validVariables.push_back(mapName);
      }

      if(SimpleParser::isExpressionCorrect(expressionAnt, validVariables))
      {
	currentExpression = expressionAnt;
	std::cout << "Texture Expression: " << currentExpression << std::endl;
	sprintf(currentExpAnt, currentExpression.c_str());
      }
      else
      {
	currentExpression = previousExpression;
	lastInvalidExp = expressionAnt;
	std::cout << "Incorrect Expression\n";
      }
    }
  }

  if(addColor)
  {
    ListColor tempListColor = { Vec3f(0, 0, 0), 1.0f, false };
    addListColor(textureBar, tempListColor);

    addColor = false;
  }

  // If expression changed end we are actually rendering expression
  // Or we toggle render expression flag
  if((currentExpression != previousExpression && renderExpression) ||
     prevRenderExpression != renderExpression)
  {
    shouldRegenerate = true;
  }

  prevRenderExpression = renderExpression;

  // Regenerating chunks if it's required
  if(shouldRegenerate)
  {
    regenerateTexture(shader);
    shouldRegenerate = false;
  }

  updateColors();
}

void
TextureModule::updateColors()
{
  auto colorIt = colorList.begin();
  while(colorIt != colorList.end())
  {
    ListColor& listColor = *colorIt;

    if(listColor.shouldDelete)
    {
      deleteListColor(textureBar, listColor.indexOnTheList);
      colorIt = colorList.erase(colorIt);
    }
    else colorIt++;
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
  vector<Vec3f> textureData;
  int textureWidth = textureResolutionX;
  int textureArea = textureWidth*textureWidth;
  textureData.resize(textureArea);

  // Setting correct expression and genData
  std::string expression;
  std::list<GenData> resultGenData;
  if(renderExpression)
  {
    expression = currentExpression;

    for(auto it = genDataMap.begin(); it != genDataMap.end(); it++)
    {
      const GenData& tempGenData = it->second;
      resultGenData.push_back(tempGenData);
    }
  }
  else
  {
    expression = "Map1";
    resultGenData.push_back(genDataMap[currentMapIndex]);
  }

  std::vector<real32> heightValues = getMap(Vec2f(), textureResolutionX, resultGenData, expression);
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
    }

    bottomColorIt = topColorIt;
  }

  return resultColor;
}

std::vector<real32>
TextureModule::getMap(Vec2f offset, int32 sideLength, std::list<GenData>& genDatas,
		      const std::string& expression)
{
  int32 numbOfVertices = -1;

  // if there should be bounds we add the vertices at the bounds
  numbOfVertices = sideLength * sideLength;

  std::vector<real32> result;
  result.resize(numbOfVertices);

  real32 stepSize = 1.0f / (sideLength - 1);
  real32 greyValue;

  Vec2f point00 = Vec2f(-0.5f, 0.5f);
  Vec2f point10 = Vec2f(0.5f, 0.5f);
  Vec2f point01 = Vec2f(-0.5f, -0.5f);
  Vec2f point11 = Vec2f(0.5f, -0.5f);

  // Parser Stuff ----------------
  SimpleParser simpleParser;
  VariableMap variableMap;
  // I set variable Map first becauuuse ???
  simpleParser.setVariableMap(&variableMap);
  EntryList reversePolish = simpleParser.getReversePolish(expression);
  // -----------------------------
  const StringList stringList = SimpleParser::getListOfVariables(reversePolish);

  std::map<uint32, Vec4f> values;
  std::vector<VariableMap> variableMapBuffer;

  std::unordered_map<int32, GenData> genDataMap;
  for(auto it = genDatas.begin(); it != genDatas.end(); it++)
  {
    int32 mapIndex = std::distance(genDatas.begin(), it);

    std::string mapString = "Map" + std::to_string(mapIndex + 1);
    if(std::find(std::begin(stringList), std::end(stringList), mapString) != std::end(stringList))
    {
      GenData& tempGenData = *it;
      genDataMap[mapIndex] = tempGenData;
    }
  }

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

      const GenData& genData = it->second;
      if(genData.noiseType == NT_PERLIN)
      {

	values[index] = Noise::sumPerlinFast(realPositions, genData.noiseParams);

	values[index] *= 0.5f;
	values[index] += 0.5f;
	values[index] *= genData.scale;
      }
      else if(genData.noiseType == NT_VALUE)
      {
	// values[index] = 1.0f;
	for(int i = 0; i < 4; i++)
	{
	  values[index][i] = Noise::sumValue(realPositions[i], genData.noiseParams);
	}
	values[index] *= genData.scale;
      }
      else if(genData.noiseType == NT_WORLEY)
      {
	// values[index] = 1.0f;
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

      for(int32 j = 0; j < genDatas.size(); j++)
      {
	std::string mapName = "Map" + std::to_string(j+1);
	variableMap[mapName] = values[j][i];
      }

      variableMap["x"] = realPositions[i].x;
      variableMap["y"] = realPositions[i].y;

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
