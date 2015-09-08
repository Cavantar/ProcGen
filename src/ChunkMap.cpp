#include "Game.h"
#include "jpb\SimpleParser.h"

bool
compareChunkData(const ChunkData& chunkData1, const ChunkData& chunkData2)
{
  if(chunkData1.distanceFromCamera < chunkData2.distanceFromCamera) return true;
  return false;
}

ChunkMap::ChunkMap()
{
  genData = { NT_PERLIN, { 0.75f, 8, 2.0f, 0.4f }, 2.0f } ;
  genDataMap[1] = genData;
  prevGenData = genData;

  genDataMap[2] = { NT_PERLIN, {0.28f, 8, 2.0f, 0.37f}, 2.0f };
  genDataMap[3] = { NT_WORLEY, { 0.5f, 1, 2.5f, 0.4f }, 2.0f };

  colorSet[0] = glm::vec4(0, 0.67, 0, 1.0);
  colorSet[1] = glm::vec4(0.8, 0.8, 0.8, 1.0);

  recalculateDetailLevels();

  currentExpression = "Map1";
  std::cout << "Terrain Expression: " << currentExpression << std::endl;

  previousExpression = currentExpression;
  sprintf(expressionAnt, currentExpression.c_str());
  sprintf(currentExpAnt, currentExpression.c_str());

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
ChunkMap::update(GLSLShader& shader, glm::vec2& playerPosition)
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
	std::cout << "Terrain Expression: " << currentExpression << std::endl;
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

  if(shouldLoadSettings)
  {
    std::cout << "Loading setttings: " << filenameAnt << std::endl;
    std::string filename = "examples/";
    filename += filenameAnt;
    filename += ".ts";

    loadState(filename);
    shouldLoadSettings = false;
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

  if(shouldSaveGeometry)
  {
    std::cout << "Saving geometry !" << std::endl;
    std::string filename = "meshes/testTwo.obj";

    int32 tempChunkWidth = 4;

    Vec2u dimensions = Vec2u(baseSideLength * tempChunkWidth, baseSideLength * tempChunkWidth);

    // Repacking genData
    GenDataList resultGenData;
    for(auto it = genDataMap.begin(); it != genDataMap.end(); it++)
    {
      const GenData& tempGenData = it->second;
      resultGenData.push_back(tempGenData);
    }

    std::vector<Vec4f>& map = Noise::getMapFast(Vec2f(0, 0),baseSideLength * tempChunkWidth, resultGenData,
						currentExpression, tempChunkWidth, true);

    Net tempNet;
    tempNet.prepareDataWithBounds(dimensions, map);
    tempNet.saveToObj(filename);

    // filename += filenameAnt;
    //filename += ".ts";

    //saveState(filename);
    shouldSaveGeometry = false;
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
    regenerateChunks();
    shouldRegenerate = false;
  }

  recalculateDetailLevels();

  std::list<ChunkData>& requiredChunksData = getChunksForPosition(playerPosition);
  generateRequiredChunks(requiredChunksData);
  deleteUnneededChunks(requiredChunksData);

  processThreads(shader);
  prevGenData = genDataMap[currentMapIndex];
}

void
ChunkMap::render(GLSLShader& shader, const RENDER_TYPE renderType, GLuint globalMatricesUBO, const CameraData& cameraData) {
  shader.use();
  glUniform4fv(shader("colorSet"), 2, (GLfloat *)colorSet);

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

  TwDefine(" Terrain/'Noise Parameters' group='Generation' ");

  TwAddVarRW(bar, "Regenerate", TW_TYPE_BOOLCPP, &shouldRegenerate,
	     " label='Regenerate' group='Generation'");


  // Presentation
  TwAddVarRW(bar, "BottomColor", TW_TYPE_COLOR4F, colorSet,
	     " label='BottomColor'  group='Presentation'");
  TwAddVarRW(bar, "TopColor", TW_TYPE_COLOR4F, &colorSet[1],
	     " label='TopColor'  group='Presentation'");
  TwAddVarRW(bar, "RenderBehind", TW_TYPE_BOOLCPP, &renderBehind,
	     " label='RenderBehind'  group='Presentation'");

  // Generation
  TwAddVarRW(bar, "MaxThreads", TW_TYPE_INT32, &maxNumbOfThreads,
	     " label='MaxThreads' min=2 max=10 step=1 keyIncr='+' keyDecr='-' group='Presentation'");
  TwAddVarRW(bar, "ChunkRadius", TW_TYPE_INT32, &chunkRadius,
	     " label='ChunkRadius' min=0 max=20 step=1 keyIncr='+' keyDecr='-' group='Presentation'");

  TwAddVarRW(bar, "Level Of Detail", TW_TYPE_BOOLCPP, &lod,
	     " label='Level Of Detail' help='Toggle Level Of Detail' group='LOD'");
  TwAddVarRW(bar, "BaseSideLength", TW_TYPE_INT32, &baseSideLength,
	     " label='BaseSideLength' min=16 max=512 step=16 keyIncr='+' keyDecr='-' group='LOD'");
  TwAddVarRW(bar, "GeometryDescRate", TW_TYPE_FLOAT, &descentionRate,
	     " label='GeometryDescentionRate' min=0.1 max=1.0 step=0.05 keyIncr='+' keyDecr='-' group='LOD'");

  TwDefine(" Terrain/LOD group='Presentation' ");

  TwAddVarRW(bar, "filename", TW_TYPE_CSSTRING(sizeof(filenameAnt)), filenameAnt, "group='Settings IO'");
  TwAddVarRW(bar, "LoadFile", TW_TYPE_BOOLCPP, &shouldLoadSettings,
	     " label='LoadFile' group='Settings IO'");
  TwAddVarRW(bar, "SaveFile", TW_TYPE_BOOLCPP, &shouldSaveSettings,
	     " label='SaveFile' group='Settings IO'");

  TwAddVarRW(bar, "SaveGeometry", TW_TYPE_BOOLCPP, &shouldSaveGeometry,
	     " label='SaveGeometry' group='Geometry Export'");

}

void
ChunkMap::showDebugInfo() const
{
  std::cout << "Numb Of Chunks Rendered: " << chunks.size() << std::endl;
  std::cout << "Numb Of Chunks Preparing: " << preparingChunks.size() << std::endl;
  //cout << "Numb Of Free Threads: " << threadsAvailable << endl;
  std::cout << "CurrentExpression: " << currentExpression << std::endl;

#if 0
  for(auto i = detailLevels.begin(); i!=detailLevels.end();i++) {
    cout << i->first << "  " << i->second << endl;
  }
#endif

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
  static bool shouldSave = true;

  int chunksToCopy = numbOfChunksPerFrame;
  auto it = preparingChunks.begin();
  while(it!= preparingChunks.end()) {
    if((*it)->ready) {
      ChunkPtr chunkPtr = *it;
      chunkPtr->joinThreadAndCopy(shader);

      // Save this thing
      if(shouldSave)
      {
	const Net& net = chunkPtr->getNet();
	net.saveToObj("meshes/test.obj");

	shouldSave = false;
      }

      if(chunks.size() == 0) {
	shader.use();
	glUniform2fv(shader("heightBounds"), 1, &chunkPtr->heightBounds.x);
	shader.unUse();
      }

      deleteChunk(glm::ivec2(chunkPtr->position_x,chunkPtr->position_y));
      chunks.push_back(chunkPtr);
      it = preparingChunks.erase(it);

      if(--chunksToCopy == 0) break;
    }
    else it++;
  }

}

std::list<ChunkData>
ChunkMap::getChunksForPosition(const glm::vec2& position) const
{
  std::list<ChunkData> requestedChunks;

  // It's subtracted because Chunks are rendered at 50,50
  glm::ivec2 normalizedPosition = glm::ivec2((int)floor((position.x - 50.0f) / 100) + 1 , -(int)floor((position.y + 50) / 100));

  requestedChunks.push_back(ChunkData(normalizedPosition,0));
  addFields(normalizedPosition, requestedChunks, chunkRadius);

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
    glm::ivec2 position = glm::ivec2((*it)->position_x, (*it)->position_y);

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
    if(glm::ivec2(chunk->position_x, chunk->position_y) == chunkData.position &&
       chunk->sideLength == getNumbOfVertForDetailLevel(chunkData.detailLevel)) return true;
  }

  // Is It Preparing
  for(auto it = preparingChunks.begin(); it != preparingChunks.end(); it++) {
    const ChunkPtr& chunk = (*it);
    if(glm::ivec2(chunk->position_x, chunk->position_y) == chunkData.position &&
       chunk->sideLength == getNumbOfVertForDetailLevel(chunkData.detailLevel)) return true;
  }

  return false;
}

void
ChunkMap::deleteChunk(const glm::ivec2& chunkPosition)
{
  for(auto it = chunks.begin(); it != chunks.end(); it++) {
    const ChunkPtr& chunk = (*it);
    if(glm::ivec2(chunk->position_x, chunk->position_y) == chunkPosition) {
      chunks.erase(it);
      return ;
    }
  }
}

void
ChunkMap::generateChunk(const ChunkData& chunkData)
{
  //cout << "Generating Chunk !\n";
  ChunkPtr chunk = ChunkPtr(new Chunk());
  preparingChunks.push_back(chunk);

  std::string expression;
  GenDataList resultGenData;
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

  chunk->startPrepareThread(chunkData.position, resultGenData, getNumbOfVertForDetailLevel(chunkData.detailLevel),
			    expression);
}

void
ChunkMap::addSurrounding(const glm::ivec2& position, std::list<glm::ivec2>& required) const
{
  required.push_back(position + glm::ivec2(1, 0));
  required.push_back(position + glm::ivec2(1, -1));
  required.push_back(position + glm::ivec2(0, -1));
  required.push_back(position + glm::ivec2(-1, -1));
  required.push_back(position + glm::ivec2(-1, 0));
  required.push_back(position + glm::ivec2(-1, 1));
  required.push_back(position + glm::ivec2(0, 1));
  required.push_back(position + glm::ivec2(1, 1));
}

void
ChunkMap::addFields(const glm::ivec2& position, std::list<ChunkData>& required, const int radius) const {
  for(int y = 0; y < radius; y++) {
    addFieldsInSquare(position, required, y);
  }
}

void
ChunkMap::addFieldsInSquare(const glm::ivec2& position, std::list<ChunkData>& required, const int distance) const {
  int squareLength = (distance + 1) * 2 + 1;
  int detailLevel = lod ? distance+1 : 0;

  for(int i = 0; i < squareLength; i++) {
    // required.push_back(ChunkData(position + glm::ivec2(-distance - 1 + i, -distance - 1), detailLevel));
    // required.push_back(ChunkData(position + glm::ivec2(-distance - 1 + i, distance + 1),detailLevel));

    int deltaX = ((i + 1)/2);
    if(i%2) deltaX *= -1;

    required.push_back(ChunkData(position + glm::ivec2(deltaX, -distance - 1), detailLevel, distance));
    required.push_back(ChunkData(position + glm::ivec2(deltaX, distance + 1), detailLevel, distance));

    if(i < squareLength - 2)
    {
      int deltaY = ((i + 1)/2);
      if(i%2) deltaY *= -1;

      required.push_back(ChunkData(position + glm::ivec2(-distance - 1, deltaY),detailLevel, distance));
      required.push_back(ChunkData(position + glm::ivec2(distance + 1 , deltaY),detailLevel, distance));
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

  glm::vec3 chunkPosition((chunk->position_x * 100) + 50, 0, (-chunk->position_y * 100) - 50);
  glm::vec3 cameraPosition(cameraData.cameraPosition);

  glm::vec3 localChunkPosition = chunkPosition - cameraPosition;
  glm::vec3 chunkDirection = glm::normalize(localChunkPosition);

  float tempDot = glm::dot(chunkDirection, -cameraData.lookVec);
  float radAngle = acos(tempDot);
  float angle = (radAngle / M_PI) * 180.0f;
  result = angle < 80.0f;

  return result;
}

void
ChunkMap::recalculateDetailLevels()
{
  int numbOfLevels = 10;
  detailLevels[0] = baseSideLength;
  for(int i = 1; i<numbOfLevels;i++) {
    detailLevels[i] = int(ceil((float)detailLevels[i-1]*descentionRate));
    if(detailLevels[i] < 5) detailLevels[i] = 5;
  }

}

void
ChunkMap::saveState(const std::string& filename)
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

  file.close();
}

void
ChunkMap::loadState(const std::string& filename)
{
  std::ifstream file;
  file.open(filename.c_str(), std::ios::in | std::ios::binary);
  file.read(expressionAnt, sizeof(expressionAnt));

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

  file.read((char*)&currentMapIndex, sizeof(int32));
  genData = genDataMap[currentMapIndex];

  file.close();
}
