#include <jpb\SimpleParser.h>
#include "Game.h"

bool
compareChunkData(const ChunkData& chunkData1, const ChunkData& chunkData2)
{
  if(chunkData1.distanceFromCamera < chunkData2.distanceFromCamera) return true;
  return false;
}

ChunkMap::ChunkMap()
{
  // recalculateDetailLevels();
  sprintf(objFilenameAnt, "");
}

ChunkMap::~ChunkMap()
{
  // Waiting for chunks that are being processed in threads and then deleting them
  while(preparingChunks.size() > 0)
  {
    auto it = preparingChunks.begin();
    while(it!= preparingChunks.end()) {
      if((*it)->ready) it = preparingChunks.erase(it);
      else it++;
    }
  }
}

void
ChunkMap::update(GLSLShader& shader, Vec2f& cameraPosition)
{
  static GenData defaultGenData = { NT_PERLIN, { 0.75f, 5, 2.0f, 0.4f }, 2.0f } ;

  // Saving Geometry
  if(shouldSaveGeometry)
  {
    std::cout << "Saving geometry !" << std::endl;
    std::string filename = "meshes/";
    filename += objFilenameAnt;
    filename += ".obj";

    int32 tempChunkWidth = chunkExportCount;
    int32 resolution = mapGenData->resolution + (chunkExportCount - 1) * mapGenData->resolution * 2 ;
    real32 regionLengthModifier = 1.0f + (chunkExportCount - 1) * 2.0f;

    Vec2u dimensions = Vec2u(resolution, resolution);

    std::string expression = mapGenData->currentExpression;
    if(!mapGenData->renderExpression)
      expression = "Map" + std::to_string(mapGenData->currentMapIndex);

    GenDataMap filteredGenDataMap = mapGenData->getFilteredGenDataMap(expression);

    Vec2f normalizedPosition = Vec2f((int)floor((cameraPosition.x - 50.0f) / 100) + 1,
				     -(int)floor((cameraPosition.y + 50) / 100));

    std::vector<Vec4f>& map = Noise::getMapFast(normalizedPosition, resolution, filteredGenDataMap,
						mapGenData->currentExpression, regionLengthModifier, true);

    Net tempNet;
    tempNet.prepareDataWithBounds(dimensions, map);
    tempNet.saveToObj(filename);

    shouldSaveGeometry = false;
  }

  // Regenerating chunks if it's required
  if(shouldRegenerate || mapGenData->shouldRegenerate)
  {
    regenerateChunks();
    shouldRegenerate = false;
  }

  recalculateDetailLevels();

  std::list<ChunkData>& requiredChunksData = getChunksForPosition(cameraPosition);
  generateRequiredChunks(requiredChunksData);
  deleteUnneededChunks(requiredChunksData);

  processThreads(shader);
}

void
ChunkMap::render(GLSLShader& shader, const RENDER_TYPE renderType, GLuint globalMatricesUBO, const CameraData& cameraData)
{
  shader.use();

  // Setting Shader Uniforms

  int32 renderOptions = 0;
  if(turnOffNormals) renderOptions |= 1;
  if(exportBoundsOn) renderOptions |= 2;
  if(clampFog) renderOptions |= 4;

  renderOptions |= ((chunkExportCount) << 16);

  glUniform1i(shader.getUniform("renderOptions"), renderOptions);
  glUniform2fv(shader.getUniform("fogBounds"), 1, (GLfloat *)&fogBounds);
  glUniform4fv(shader.getUniform("fogColor"), 1, (GLfloat *)&fogColor);

  // Setting Colors
  const ColorList& colorList = mapGenData->colorList;

  int32 currentIndex = 0;
  for(auto it = colorList.begin(); it != colorList.end(); it++)
  {
    const ListColor& color = *it;
    std::string colorPre = "colors[" + std::to_string(currentIndex) + "].";

    Vec4f color4 = Vec4f(color.color.x, color.color.y, color.color.z, 1.0f);
    glUniform4fv(shader.getUniform(colorPre + "color"), 1, (GLfloat *)&color4);
    glUniform1f(shader.getUniform(colorPre + "startValue"), color.startValue);

    ++currentIndex;
  }

  for(auto it = chunks.begin(); it != chunks.end(); it++)
  {
    const ChunkPtr& chunk = *it;

    // Check If Chunk Should Be Rendered
    if(renderBehind || shouldChunkBeRendered(chunk, cameraData))
    {
      chunk->render(shader, renderType, globalMatricesUBO);
    }
  }
  shader.unUse();
}

void
ChunkMap::setTweakBar(TwBar * const bar)
{

  TwAddVarRW(bar, "Regenerate", TW_TYPE_BOOLCPP, &shouldRegenerate,
	     " label='Regenerate' group='TerrainGen'");

  // Presentation
  TwAddVarRW(bar, "RenderBehind", TW_TYPE_BOOLCPP, &renderBehind,
	     " label='RenderBehind'  group='Presentation'");

  TwAddVarRW(bar, "TurnOffNormals", TW_TYPE_BOOLCPP, &turnOffNormals,
	     " label='TurnOffNormals'  group='Presentation'");

  TwAddVarRW(bar, "fogColor", TW_TYPE_COLOR4F, &fogColor,
	     " label='fogColor'  group='Presentation'");

  TwAddVarRW(bar, "ClampFog", TW_TYPE_BOOLCPP, &clampFog,
	     " label='ClampFog'  group='Presentation'");

  TwAddVarRW(bar, "FogNear", TW_TYPE_FLOAT, &fogBounds.x,
	     " label='FogNear' min=100 max=100000 step=10 keyIncr='+' keyDecr='-' group='Presentation'");
  TwAddVarRW(bar, "FogFar", TW_TYPE_FLOAT, &fogBounds.y,
	     " label='FogFar' min=100 max=100000 step=10 keyIncr='+' keyDecr='-' group='Presentation'");

  TwAddVarRW(bar, "MaxThreads", TW_TYPE_INT32, &maxNumbOfThreads,
	     " label='MaxThreads' min=2 max=10 step=1 keyIncr='+' keyDecr='-' group='Presentation'");
  TwAddVarRW(bar, "ChunkRadius", TW_TYPE_INT32, &chunkRadius,
	     " label='ChunkRadius' min=1 max=20 step=1 keyIncr='+' keyDecr='-' group='Presentation'");

  TwDefine(" MapGen/'Presentation' group='TerrainGen' ");

  TwAddVarRW(bar, "Level Of Detail", TW_TYPE_BOOLCPP, &lod,
	     " label='Level Of Detail' help='Toggle Level Of Detail' group='LOD'");

  TwAddVarRW(bar, "GeometryDescRate", TW_TYPE_FLOAT, &descentionRate,
	     " label='GeometryDescentionRate' min=0.1 max=1.0 step=0.05 keyIncr='+' keyDecr='-' group='LOD'");

  TwDefine(" MapGen/'LOD' group='TerrainGen' ");

  TwAddVarRW(bar, "ObjFile", TW_TYPE_CSSTRING(sizeof(objFilenameAnt)), objFilenameAnt, "group='GeometryExport'");
  TwAddVarRW(bar, "ChunkExportCount", TW_TYPE_INT32, &chunkExportCount,
	     " label='ChunkExportCount' min=1 max=32 step=1 keyIncr='+' keyDecr='-' group='GeometryExport'");

  TwAddVarRW(bar, "ExportBoundsOn", TW_TYPE_BOOLCPP, &exportBoundsOn,
	     " label='ExportBoundsOn'  group='GeometryExport'");

  TwAddVarRW(bar, "SaveGeometry", TW_TYPE_BOOLCPP, &shouldSaveGeometry,
	     " label='SaveGeometry' group='GeometryExport'");


  TwDefine(" MapGen/'GeometryExport' group='TerrainGen' ");

  TwDefine(" MapGen/'TerrainGen' opened=false ");

}

void
ChunkMap::showDebugInfo() const
{
  std::cout << "Numb Of Chunks Rendered: " << chunks.size() << std::endl;
  std::cout << "Numb Of Chunks Preparing: " << preparingChunks.size() << std::endl;
  //cout << "Numb Of Free Threads: " << threadsAvailable << endl;
  // std::cout << "CurrentExpression: " << currentExpression << std::endl;
}

void
ChunkMap::cleanUp(GLSLShader& shader)
{
  while(preparingChunks.size() > 0)
  {
    Sleep(10);
    processThreads(shader);
  }
}

void
ChunkMap::processThreads(GLSLShader& shader)
{
  static int numbOfChunksPerFrame = 1;

  int chunksToCopy = numbOfChunksPerFrame;
  auto it = preparingChunks.begin();
  while(it!= preparingChunks.end()) {
    if((*it)->ready) {
      ChunkPtr chunkPtr = *it;
      chunkPtr->joinThreadAndCopy(shader);

      // If chunk already exist's delete it first
      deleteChunk(chunkPtr->position);
      chunks.push_back(chunkPtr);
      it = preparingChunks.erase(it);

      if(--chunksToCopy == 0) break;
    }
    else it++;
  }

}

std::list<ChunkData>
ChunkMap::getChunksForPosition(const Vec2f& position) const
{
  std::list<ChunkData> requestedChunks;

  // It's subtracted because Chunks are rendered at 50,50
  Vec2i normalizedPosition = Vec2i((int)floor((position.x - 50.0f) / 100) + 1 , -(int)floor((position.y + 50) / 100));

  requestedChunks.push_back(ChunkData(normalizedPosition,0));
  addFields(normalizedPosition, requestedChunks, chunkRadius - 1);

  // Sorting - in function of distance from camera
  requestedChunks.sort(compareChunkData);

  return requestedChunks;
}

void
ChunkMap::generateRequiredChunks(const std::list<ChunkData>& requiredChunks)
{
  for(auto it = requiredChunks.begin(); it != requiredChunks.end(); it++) {
    const ChunkData& chunkData = *it;

    if(!doesChunkExists(chunkData) && (int)preparingChunks.size() < maxNumbOfThreads-1) {
      generateChunk(chunkData);
    }
  }
}

void
ChunkMap::regenerateChunks()
{
  chunks.clear();
}

void
ChunkMap::deleteUnneededChunks(const std::list<ChunkData>& requiredChunks)
{
  auto it = chunks.begin();
  while(it != chunks.end()) {
    bool shouldStay = false;
    Vec2i position = (*it)->position;

    for(auto i = requiredChunks.begin(); i != requiredChunks.end(); i++) {
      if(i->position == position) shouldStay = true;
    }

    if(!shouldStay) it = chunks.erase(it);
    else it++;
  }
}

bool
ChunkMap::doesChunkExists(const ChunkData& chunkData)
{
  // Is It Done
  for(auto it = chunks.begin(); it != chunks.end(); it++) {
    const ChunkPtr& chunk = (*it);
    if(chunk->position == chunkData.position &&
       chunk->sideLength == getNumbOfVertForDetailLevel(chunkData.detailLevel)) return true;
  }

  // Is It Preparing
  for(auto it = preparingChunks.begin(); it != preparingChunks.end(); it++) {
    const ChunkPtr& chunk = (*it);
    if(chunk->position == chunkData.position &&
       chunk->sideLength == getNumbOfVertForDetailLevel(chunkData.detailLevel)) return true;
  }

  return false;
}

void
ChunkMap::deleteChunk(const Vec2i& chunkPosition)
{
  for(auto it = chunks.begin(); it != chunks.end(); it++) {
    const ChunkPtr& chunk = (*it);
    if(chunk->position == chunkPosition) {
      chunks.erase(it);
      return ;
    }
  }
}

void
ChunkMap::generateChunk(const ChunkData& chunkData)
{
  ChunkPtr chunk = ChunkPtr(new Chunk());
  preparingChunks.push_back(chunk);

  std::string expression = mapGenData->currentExpression;
  if(!mapGenData->renderExpression)
    expression = "Map" + std::to_string(mapGenData->currentMapIndex);

  GenDataMap filteredGenDataMap = mapGenData->getFilteredGenDataMap(expression);
  chunk->startPrepareThread(chunkData.position, filteredGenDataMap, getNumbOfVertForDetailLevel(chunkData.detailLevel),
			    expression);

}

void
ChunkMap::addSurrounding(const Vec2i& position, std::list<Vec2i>& required) const
{
  required.push_back(position + Vec2i(1, 0));
  required.push_back(position + Vec2i(1, -1));
  required.push_back(position + Vec2i(0, -1));
  required.push_back(position + Vec2i(-1, -1));
  required.push_back(position + Vec2i(-1, 0));
  required.push_back(position + Vec2i(-1, 1));
  required.push_back(position + Vec2i(0, 1));
  required.push_back(position + Vec2i(1, 1));
}

void
ChunkMap::addFields(const Vec2i& position, std::list<ChunkData>& required, const int radius) const
{
  for(int y = 0; y < radius; y++) {
    addFieldsInSquare(position, required, y);
  }
}

void
ChunkMap::addFieldsInSquare(const Vec2i& position, std::list<ChunkData>& required, const int distance) const
{
  int squareLength = (distance + 1) * 2 + 1;
  int detailLevel = lod ? distance+1 : 0;

  for(int i = 0; i < squareLength; i++) {

    int deltaX = ((i + 1)/2);
    if(i%2) deltaX *= -1;

    required.push_back(ChunkData(position + Vec2i(deltaX, -distance - 1), detailLevel, distance));
    required.push_back(ChunkData(position + Vec2i(deltaX, distance + 1), detailLevel, distance));

    if(i < squareLength - 2)
    {
      int deltaY = ((i + 1)/2);
      if(i%2) deltaY *= -1;

      required.push_back(ChunkData(position + Vec2i(-distance - 1, deltaY),detailLevel, distance));
      required.push_back(ChunkData(position + Vec2i(distance + 1 , deltaY),detailLevel, distance));
    }
  }
}

int
ChunkMap::getNumbOfVertForDetailLevel(const int detailLevel)
{
  if(detailLevels.count(detailLevel)) return detailLevels[detailLevel];
  else return 5;
}

bool
ChunkMap::shouldChunkBeRendered(const ChunkPtr chunk, const CameraData& cameraData) const
{
  bool result;

  Vec3f chunkPosition((chunk->position.x * 100) + 50, 0, (-chunk->position.y * 100) - 50);
  Vec3f cameraPosition(cameraData.cameraPosition);

  Vec3f localChunkPosition = chunkPosition - cameraPosition;
  Vec3f chunkDirection = Vec3f::normalize(localChunkPosition);

  float tempDot = Vec3f::dotProduct(chunkDirection, -cameraData.lookVec);
  float radAngle = acos(tempDot);
  float angle = (radAngle / M_PI) * 180.0f;
  result = angle < 80.0f;

  return result;
}

void
ChunkMap::recalculateDetailLevels()
{
  int numbOfLevels = 10;
  detailLevels[0] = mapGenData->resolution;
  for(int i = 1; i<numbOfLevels;i++) {
    detailLevels[i] = int(ceil((float)detailLevels[i-1]*descentionRate));
    if(detailLevels[i] < 5) detailLevels[i] = 5;
  }

}

void
MapGenData::initialize(TwBar* bar)
{
  genData = { NT_PERLIN, { 0.75f, 8, 2.0f, 0.4f }, 2.0f } ;
  genDataMap[1] = genData;
  prevGenData = genData;

  genDataMap[2] = { NT_PERLIN, {0.28f, 8, 2.0f, 0.37f}, 2.0f };
  genDataMap[3] = { NT_WORLEY, { 0.5f, 1, 2.5f, 0.4f }, 2.0f };

  currentExpression = "Map1";
  std::cout << "Terrain Expression: " << currentExpression << std::endl;

  previousExpression = currentExpression;
  sprintf(expressionAnt, currentExpression.c_str());
  sprintf(currentExpAnt, currentExpression.c_str());

  sprintf(filenameAnt, "");

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

  TwAddVarRW(bar, "Resolution", TW_TYPE_INT32, &resolution,
	     " label='Resolution' min=16 max=512 step=16 keyIncr='+' keyDecr='-' group='Generation'");

  TwDefine(" MapGen/'Noise Parameters' group='Generation' ");

  TwAddVarRW(bar, "AddColor", TW_TYPE_BOOLCPP, &addColor,
	     " label='AddColor' group='Colors'");

  TwAddSeparator(bar, NULL, " group='Colors' ");

  // Adding Default Colors

  addListColor(bar, {Vec3f(0, 0.67, 0), 0, false});
  addListColor(bar, {Vec3f(0.8, 0.8, 0.8), 1.3, false});
  addListColor(bar, {Vec3f(0.8, 0.8, 0.8), 2.0, false});

  prevColorList = colorList;

  TwAddVarRW(bar, "Filename", TW_TYPE_CSSTRING(sizeof(filenameAnt)), filenameAnt, "group='Settings IO'");
  TwAddVarRW(bar, "LoadFile", TW_TYPE_BOOLCPP, &shouldLoadSettings,
	     " label='LoadFile' group='Settings IO'");
  TwAddSeparator(bar, NULL, " group='Settings IO' ");
  TwAddVarRW(bar, "SaveFile", TW_TYPE_BOOLCPP, &shouldSaveSettings,
	     " label='SaveFile' group='Settings IO'");

  TwDefine(" MapGen/'Colors' opened=false ");
}

void
MapGenData::update(TwBar * const bar)
{
  // Regenerating chunks if it's required

  // Just To Flip the flag back after the frame
  shouldRegenerate = false;
  colorChanged = false;

  static GenData defaultGenData = { NT_PERLIN, { 0.75f, 5, 2.0f, 0.4f }, 2.0f } ;

  // If selected mapIndex changed
  if(prevMapIndex != currentMapIndex)
  {
    // And the current genData specified by index doesn't exist create it and copy the default one
    if(genDataMap.count(currentMapIndex) == 0)
    {
      genDataMap[currentMapIndex] = defaultGenData;
    }

    // Setting current genData as specified by index (cause it's changed)
    genData = genDataMap[currentMapIndex];

    // If we're not rendering expression and map index changed we have to regenerate map
    if(!renderExpression) shouldRegenerate = true;
  }
  prevMapIndex = currentMapIndex;

  // if settings changed we update genData in genDataMap and regenerate chunks
  if(genData != genDataMap[currentMapIndex])
  {
    genDataMap[currentMapIndex] = genData;
    shouldRegenerate = true;
  }

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
    ListColor tempListColor = { Vec3f(0, 0, 0), 0, false };
    addListColor(bar, tempListColor);

    addColor = false;
  }

  // If expression changed end we are actually rendering expression
  // Or we toggle render expression flag
  if((currentExpression != previousExpression && renderExpression) ||
     prevRenderExpression != renderExpression ||
     prevResolution != resolution )
  {
    shouldRegenerate = true;
  }

  prevResolution = resolution;
  prevRenderExpression = renderExpression;

  if(prevColorList != colorList) colorChanged = true;
  prevColorList = colorList;

  if(shouldLoadSettings)
  {
    std::cout << "Loading setttings: " << filenameAnt << std::endl;
    std::string filename = "examples/";
    filename += filenameAnt;
    filename += ".ts";

    loadState(bar, filename);
    prevColorList = colorList;
    shouldLoadSettings = false;
    shouldRegenerate = true;
  }

  if(shouldSaveSettings)
  {
    std::cout << "Saving setttings: " << filenameAnt << std::endl;

    std::string filename = "examples/";
    filename += filenameAnt;
    filename += ".ts";

    saveState(filename);
    shouldSaveSettings = false;
  }

  updateColors(bar);
  prevGenData = genData;
}

void
MapGenData::addListColor(TwBar * const bar, ListColor newListColor)
{
  newListColor.indexOnTheList = maxColorIndex;
  colorList.push_back(newListColor);
  ListColor& listColor = colorList.back();
  std::string colorName = ("Color: " + std::to_string(maxColorIndex));

  std::string colorProperties = " label='" + colorName + "'  group='Colors'";

  TwAddVarRW(bar, colorName.c_str(), TW_TYPE_COLOR3F, &listColor.color,
	     colorProperties.c_str());

  TwAddVarRW(bar, ("StartValue: " + std::to_string(maxColorIndex)).c_str(), TW_TYPE_FLOAT, &listColor.startValue,
	     " label='StartValue' min=0 max=10 step=0.1 keyIncr='+' keyDecr='-' group='Colors'");

  TwAddVarRW(bar, ("InsertColor: " + std::to_string(maxColorIndex)).c_str(), TW_TYPE_BOOLCPP, &listColor.insertColor,
	     " label='InsertColor' group='Colors'");

  TwAddVarRW(bar, ("DeleteColor: " + std::to_string(maxColorIndex)).c_str(), TW_TYPE_BOOLCPP, &listColor.shouldDelete,
	     " label='DeleteColor' group='Colors'");

  TwAddSeparator(bar, ("ColorSep: " + std::to_string(maxColorIndex)).c_str(), " group='Colors' ");

  ++maxColorIndex;
}

void
MapGenData::deleteListColor(TwBar * const bar, int32 colorIndex)
{
  std::string colorIndexString = std::to_string(colorIndex);

  TwRemoveVar(bar, ("Color: " + colorIndexString).c_str());
  TwRemoveVar(bar, ("StartValue: " + colorIndexString).c_str());
  TwRemoveVar(bar, ("InsertColor: " + colorIndexString).c_str());
  TwRemoveVar(bar, ("DeleteColor: " + colorIndexString).c_str());
  TwRemoveVar(bar, ("ColorSep: " + colorIndexString).c_str());
}

void
MapGenData::updateColors(TwBar * const bar)
{
  auto colorIt = colorList.begin();
  while(colorIt != colorList.end())
  {
    ListColor& listColor = *colorIt;

    if(listColor.insertColor)
    {
      std::cout << "Inserting color upwards of: " << listColor.indexOnTheList << std::endl;
      listColor.insertColor = false;

      // I'm doing this weird thing because I can't insert
      // elements in before other elements in tweak bar

      // Algorithm
      // Call deleteListColor for every element from current iterator to end
      //
      // otherwise just insert before

      ColorList elementsInFront;

      for(auto deleteIt = colorIt; deleteIt != colorList.end(); deleteIt++)
      {
	const ListColor& deleteColor = *deleteIt;
	deleteListColor(bar, deleteColor.indexOnTheList);
	elementsInFront.push_back(deleteColor);
      }
      // Deleting from the list
      colorList.erase(colorIt, colorList.end());

      // Inserting new color
      ListColor defaultColor = { Vec3f(0, 0, 0), 0, false };
      addListColor(bar, defaultColor);

      for(auto addIt = elementsInFront.begin(); addIt != elementsInFront.end(); addIt++)
      {
	const ListColor& addColor = *addIt;
	addListColor(bar, addColor);
      }

      // I'm assuming i can add only insert one color per Frame
      break;
    }

    if(listColor.shouldDelete)
    {
      deleteListColor(bar, listColor.indexOnTheList);
      colorIt = colorList.erase(colorIt);
    }
    else colorIt++;
  }
}

void
MapGenData::saveState(const std::string& filename)
{
  std::ofstream file;
  file.open(filename.c_str(), std::ios::out | std::ios::binary);
  file.write(expressionAnt, sizeof(expressionAnt));

  // Saving genDataMap

  int32 numbOfGenDatas = genDataMap.size();
  file.write((char*)&numbOfGenDatas, sizeof(int32));

  for(auto it = genDataMap.begin(); it != genDataMap.end(); it++)
  {
    int32 mapIndex = it->first;
    const GenData& currentGenData = it->second;

    file.write((char*)&mapIndex, sizeof(int32));
    file.write((char*)&currentGenData, sizeof(GenData));
  }

  file.write((char*)&currentMapIndex, sizeof(int32));

  // Saving Color Stuff

  int32 tempColorIndex = 1;

  int32 numbOfColors = colorList.size();
  file.write((char*)&numbOfColors, sizeof(int32));
  for(auto it = colorList.begin(); it != colorList.end(); it++)
  {
    ListColor listColor = *it;
    listColor.indexOnTheList = tempColorIndex;

    file.write((char*)&listColor, sizeof(ListColor));
    tempColorIndex++;
  }


  file.close();
}

void
MapGenData::loadState(TwBar * const bar, const std::string& filename)
{
  std::ifstream file;
  file.open(filename.c_str(), std::ios::in | std::ios::binary);
  if(!file.is_open())
  {
    std::cout << " Coudln't open file: " << filename << std::endl;
    return ;
  }
  file.read(expressionAnt, sizeof(expressionAnt));

  currentExpression = expressionAnt;
  previousExpression = currentExpression;

  int32 numbOfGenDatas = 0;
  file.read((char *)&numbOfGenDatas, sizeof(int32));
  for(int i = 0; i < numbOfGenDatas; i++)
  {
    int32 mapIndex = 0;
    GenData currentGenData;

    file.read((char*)&mapIndex, sizeof(int32));
    file.read((char*)&currentGenData, sizeof(GenData));

    genDataMap[mapIndex] = currentGenData;
  }


  // Deleting old colors from tweak bar
  for(auto it = colorList.begin(); it != colorList.end(); it++)
  {
    ListColor& listColor = *it;
    deleteListColor(bar, listColor.indexOnTheList);
  }
  colorList.clear();

  maxColorIndex = 1;
  // -------------

  file.read((char*)&currentMapIndex, sizeof(int32));
  genData = genDataMap[currentMapIndex];

  int32 numbOfColors = 0;
  file.read((char*)&numbOfColors, sizeof(int32));
  for(int i = 0; i < numbOfColors; i++)
  {
    ListColor listColor;
    file.read((char*)&listColor, sizeof(ListColor));
    addListColor(bar, listColor);
  }


  file.close();
}

GenDataMap
MapGenData::getFilteredGenDataMap(const std::string& expression) const
{
  SimpleParser tempParser;
  EntryList& reversePolish = tempParser.getReversePolish(expression);
  StringList requiredVars = SimpleParser::getListOfVariables(reversePolish);

  GenDataMap filteredGenDataMap;
  for(auto it = requiredVars.begin(); it != requiredVars.end(); it++)
  {
    const std::string& varName = *it;
    if(varName.compare(0, 3, "Map") == 0)
    {
      int32 mapIndex = std::stoi(varName.substr(3));
      filteredGenDataMap[mapIndex] = genDataMap.at(mapIndex);
    }
  }

  return filteredGenDataMap;
}
