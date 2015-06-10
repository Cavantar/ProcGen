#include "Game.h"

TwEnumVal ChunkMap::mapEV[4]= {
  { MAP1, "Map1"},
  { MAP2, "Map2"},
  { MAP3, "Map3"},
  { MAP1XMAP2, "Map1 x Map2"}
};

bool compareChunkData(const ChunkData& chunkData1, const ChunkData& chunkData2)
{
  if(chunkData1.distanceFromCamera < chunkData2.distanceFromCamera) return true;
  return false;
}

// Frequency: 0.6, Octaves: 5.0, Lacunarity: 3.0 || 2.6, Persistence: 0.3
// Frequency: 0.8, Octaves: 5.0, Lacunarity: 2.0, Persistence: 0.3
// Frequency: 0.5, Octaves: 5.0, Lacunarity: 2.6, Persistence: 0.3

// Base
// 1) Frequency: 0.3, Octaves: 5.0, Lacunarity: 2.0, Persistence: 0.6

ChunkMap::ChunkMap() {
  genData = { NT_PERLIN, { 0.75f, 5, 2.0f, 0.4f }, 2.0f } ; 
  genDataMap[1] = genData;
  prevGenData = genData;
  
  genDataMap[2] = { NT_PERLIN, {0.2f, 5, 2.0f, 0.4f}, 2.0f };
  genDataMap[3] = { NT_PERLIN, { 0.3f, 3, 2.5f, 0.4f }, 2.0f };
    
  colorSet[0] = glm::vec4(0, 0.67, 0, 1.0);
  colorSet[1] = glm::vec4(0.8, 0.8, 0.8, 1.0);
  
  recalculateDetailLevels();
}
ChunkMap::~ChunkMap(){
  //  cout << preparing
  // ChunkData.position
}

void ChunkMap::process(GLSLShader& shader, glm::vec2& playerPosition) {

  if(prevMapTypeShow != currentMapTypeShow)
  {
    chunks.clear();
  }
  prevMapTypeShow = currentMapTypeShow;
  
  if(prevMapTypeEdit != currentMapTypeEdit)
  {
    genData = genDataMap[currentMapTypeEdit];
    chunks.clear();
  }
  prevMapTypeEdit = currentMapTypeEdit;
  
  if(didSettingsChange())
  {
    genDataMap[currentMapTypeEdit] = genData;
    chunks.clear();
  }
  recalculateDetailLevels();
  
  list<ChunkData>& requiredChunksData = getChunksForPosition(playerPosition);
  generateRequiredChunks(requiredChunksData);
  deleteUnneededChunks(requiredChunksData);
  
  checkChunks(shader);
  prevGenData = genDataMap[currentMapTypeEdit];
}

void ChunkMap::render(GLSLShader& shader, const RENDER_TYPE renderType, GLuint globalMatricesUBO, const CameraData& cameraData) {
  shader.use();
  glUniform4fv(shader("colorSet"), 2, (GLfloat *)colorSet);
  
  for(auto i = chunks.begin(); i != chunks.end(); i++)
  {
    // Check If Chunk Should Be Rendered
    if(renderBehind || shouldChunkBeRendered((*i), cameraData))
    {
      (*i)->render(shader, renderType, globalMatricesUBO);
    }
  }
  shader.unUse();
}

void ChunkMap::setTweakBar(TwBar * const bar) {
  
  mapTypeEnum = TwDefineEnum("MapType", mapEV, 4);
  TwAddVarRW(bar, "MapTypeShow", mapTypeEnum, &currentMapTypeShow, NULL);
  TwAddVarRW(bar, "MapTypeEdit", mapTypeEnum, &currentMapTypeEdit, NULL);
  
  // Noise Parameters
  TwAddVarRW(bar, "Perlin Noise", TW_TYPE_BOOLCPP, &isPerlin,
	     " label='PerlinNoise' help='Toggle Level Of Detail' group='Noise Parameters'");
  TwAddVarRW(bar, "Frequency", TW_TYPE_FLOAT, &genData.noiseParams.frequency,
	     " label='Frequency' min=-0.050 max=100 step=0.025 keyIncr='+' keyDecr='-' help='Increase/decrease the frequency.' group='Noise Parameters'");
  TwAddVarRW(bar, "Octaves", TW_TYPE_INT32, &genData.noiseParams.octaves,
	     " label='Octaves' min=1 max=10 step=1 keyIncr='+' keyDecr='-' help='Increase/decrease number of octaves.' group='Noise Parameters'");
  TwAddVarRW(bar, "Lacunarity", TW_TYPE_FLOAT, &genData.noiseParams.lacunarity,
	     " label='Lacunarity' min=1.1 max=10.0 step=0.05 keyIncr='+' keyDecr='-' help='Increase/decrease the lacunarity.' group='Noise Parameters'");
  TwAddVarRW(bar, "Persistence", TW_TYPE_FLOAT, &genData.noiseParams.persistence,
	     " label='Persistence' min=0.05 max=1.0 step=0.05 keyIncr='+' keyDecr='-' help='Increase/decrease the persistence.' group='Noise Parameters'");
  TwAddVarRW(bar, "Scale", TW_TYPE_FLOAT, &genData.scale,
	     " label='Scale' min=0.1 max=50.0 step=0.1 keyIncr='+' keyDecr='-' help='Increase/decrease the scale.' group='Noise Parameters'");
  
  // Presentation
  TwAddVarRW(bar, "BottomColor", TW_TYPE_COLOR4F, colorSet,
	     " label='BottomColor' help='ChangeColor.' group='Presentation'");
  TwAddVarRW(bar, "TopColor", TW_TYPE_COLOR4F, &colorSet[1],
	     " label='TopColor' help='ChangeColor.' group='Presentation'");
  TwAddVarRW(bar, "RenderBehind", TW_TYPE_BOOLCPP, &renderBehind,
	     " label='RenderBehind' help='Toggle Level Of Detail' group='Presentation'");
  // Generation
  TwAddVarRW(bar, "MaxThreads", TW_TYPE_INT32, &maxNumbOfThreads,
	     " label='MaxThreads' min=2 max=10 step=1 keyIncr='+' keyDecr='-' help='Increase/decrease number of threads Used.' group='Presentation'");
  TwAddVarRW(bar, "ChunkRadius", TW_TYPE_INT32, &chunkRadius,
	     " label='ChunkRadius' min=0 max=20 step=1 keyIncr='+' keyDecr='-' help='Increase/decrease chunk rendering radius.' group='Presentation'");
  TwAddVarRW(bar, "Level Of Detail", TW_TYPE_BOOLCPP, &lod,
	     " label='Level Of Detail' help='Toggle Level Of Detail' group='Presentation'");
  TwAddVarRW(bar, "BaseSideLength", TW_TYPE_INT32, &baseSideLength,
	     " label='BaseSideLength' min=16 max=512 step=16 keyIncr='+' keyDecr='-' help='Increase/decrease chunk base side length.' group='Presentation'");
  TwAddVarRW(bar, "GeometryDescRate", TW_TYPE_FLOAT, &descentionRate,
	     " label='GeometryDescentionRate' min=0.1 max=1.0 step=0.05 keyIncr='+' keyDecr='-' help='Increase/decrease the GeometryDescentionRate.' group='Presentation'");
  
}

void ChunkMap::showDebugInfo() const {
  cout << "Numb Of Chunks Rendered: " << chunks.size() << endl;
  cout << "Numb Of Chunks Preparing: " << preparingChunks.size() << endl;
  //cout << "Numb Of Free Threads: " << threadsAvailable << endl;
  
#if 0
  for(auto i = detailLevels.begin(); i!=detailLevels.end();i++) {
    cout << i->first << "  " << i->second << endl;
  }
#endif
  
}

void ChunkMap::checkChunks(GLSLShader& shader) {
  static int numbOfChunksPerFrame = 1;

  int chunksToCopy = numbOfChunksPerFrame; 
  auto it = preparingChunks.begin();
  while(it!= preparingChunks.end()) {
    if((*it)->ready) {
      ChunkPtr chunkPtr = *it;
      chunkPtr->joinThreadAndCopy(shader);
      
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

bool ChunkMap::didSettingsChange() {
  
  if(isPerlin) genData.noiseType = NT_PERLIN;
  else genData.noiseType = NT_VALUE;
  
  return genData != genDataMap[currentMapTypeEdit];
}

list<ChunkData> ChunkMap::getChunksForPosition(const glm::vec2& position) const {
  list<ChunkData> requestedChunks;
  
  // It's subtracted because Chunks are rendered at 50,50 
  glm::ivec2 normalizedPosition = glm::ivec2((int)floor((position.x - 50.0f) / 100) + 1 , -(int)floor((position.y + 50) / 100));
  requestedChunks.push_back(ChunkData(normalizedPosition,0));
  addFields(normalizedPosition, requestedChunks, chunkRadius);
  requestedChunks.sort(compareChunkData);
  
  return requestedChunks;
}

void ChunkMap::generateRequiredChunks(const list<ChunkData>& requiredChunks) {
  for(auto i = requiredChunks.begin(); i != requiredChunks.end(); i++) {
    if(!doesChunkExists(*i) && (int)preparingChunks.size() < maxNumbOfThreads-1) {
      generateChunk(*i);
    }
  }
}

void ChunkMap::deleteUnneededChunks(const list<ChunkData>& requiredChunks) {
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

bool ChunkMap::doesChunkExists(const ChunkData& chunkData) {
  // Is It Done
  for(auto i = chunks.begin(); i != chunks.end(); i++) {
    if(glm::ivec2((*i)->position_x, (*i)->position_y) == chunkData.position && (*i)->sideLength == getNumbOfVertForDetailLevel(chunkData.detailLevel)) return true;
  }
  // Is It Preparing
  for(auto i = preparingChunks.begin(); i != preparingChunks.end(); i++) {
    if(glm::ivec2((*i)->position_x, (*i)->position_y) == chunkData.position && (*i)->sideLength == getNumbOfVertForDetailLevel(chunkData.detailLevel)) return true;
  }
  return false;
}

void ChunkMap::deleteChunk(const glm::ivec2& chunkPosition) {
  for(auto i = chunks.begin(); i != chunks.end(); i++) {
    if(glm::ivec2((*i)->position_x, (*i)->position_y) == chunkPosition) {
      chunks.erase(i);
      return ;
    }
  }
}

void ChunkMap::generateChunk(const ChunkData& chunkData) {
  //cout << "Generating Chunk !\n";
  ChunkPtr chunk = ChunkPtr(new Chunk());
  preparingChunks.push_back(chunk);
  
  // -------------------
  // GenData Is Member Variable !!!
  
  GenDataList resultGenData;
  switch(currentMapTypeShow)
  {
  case MAP1:
  case MAP2:
  case MAP3:
    {
      resultGenData.push_back(genDataMap[currentMapTypeShow]);
    }
    break;
  case MAP1XMAP2:
    {
      resultGenData.push_back(genDataMap[1]);
      resultGenData.push_back(genDataMap[2]);
    }
    break;
  }
  // resultGenData.push_back(genDataMap[1]);
  chunk->startPrepareThread(chunkData.position, resultGenData, getNumbOfVertForDetailLevel(chunkData.detailLevel));
  //cout << "position.x " << position.x << " position.y: " << position.y << endl;
  //ThreadsAvailable--;
}

void ChunkMap::addSurrounding(const glm::ivec2& position, list<glm::ivec2>& required) const {
  required.push_back(position + glm::ivec2(1, 0));
  required.push_back(position + glm::ivec2(1, -1));
  required.push_back(position + glm::ivec2(0, -1));
  required.push_back(position + glm::ivec2(-1, -1));
  required.push_back(position + glm::ivec2(-1, 0));
  required.push_back(position + glm::ivec2(-1, 1));
  required.push_back(position + glm::ivec2(0, 1));
  required.push_back(position + glm::ivec2(1, 1));
}

void ChunkMap::addFields(const glm::ivec2& position, list<ChunkData>& required, const int radius) const {
  for(int y = 0; y < radius; y++) {
    addFieldsInSquare(position, required, y);
  }
}

void ChunkMap::addFieldsInSquare(const glm::ivec2& position, list<ChunkData>& required, const int distance) const {
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

int ChunkMap::getNumbOfVertForDetailLevel(const int detailLevel) {
  if(detailLevels.count(detailLevel)) return detailLevels[detailLevel];
  else return 5;
}

bool ChunkMap::shouldChunkBeRendered(const ChunkPtr chunk, const CameraData& cameraData) const
{
  bool result;
  
#if 0
  
  glm::vec2 chunkPosition((chunk->position_x * 100) + 50, (-chunk->position_y * 100) - 50);
  glm::vec2 cameraPosition = glm::vec2(cameraData.cameraPosition.x, cameraData.cameraPosition.z);
  
  glm::vec2 localChunkPosition = chunkPosition - cameraPosition;
  
  result = glm::dot(localChunkPosition, glm::vec2(cameraData.lookVec.x, cameraData.lookVec.z)) < 0;
  
#else
  glm::vec3 chunkPosition((chunk->position_x * 100) + 50, 0, (-chunk->position_y * 100) - 50);
  glm::vec3 cameraPosition(cameraData.cameraPosition);
  
  glm::vec3 localChunkPosition = chunkPosition - cameraPosition;
  glm::vec3 chunkDirection = glm::normalize(localChunkPosition);
  
  float tempDot = glm::dot(chunkDirection, -cameraData.lookVec);
  float radAngle = acos(tempDot);
  float angle = (radAngle / M_PI) * 180.0f;
  result = angle < 80.0f;
#endif
  return result;
}

void ChunkMap::recalculateDetailLevels(){
  int numbOfLevels = 10;
  detailLevels[0] = baseSideLength;
  for(int i = 1; i<numbOfLevels;i++) {
    detailLevels[i] = int(ceil((float)detailLevels[i-1]*descentionRate));
    if(detailLevels[i] < 5) detailLevels[i] = 5;
  }

}
