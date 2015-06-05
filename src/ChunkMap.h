#pragma once 
#include "Includes.h"
#include "Chunk.h"

typedef shared_ptr <Chunk> ChunkPtr ;


// Used Mostly For Requesting Chunks Based On The Criteria Present as Fields
class ChunkData{
public:
  glm::ivec2 position;
  int detailLevel;
  
  ChunkData(const glm::ivec2 position, const int detailLevel): position(position), detailLevel(detailLevel){}
};

class ChunkMap{
public:
  ChunkMap();
  ~ChunkMap();
  void process(GLSLShader& shader, glm::vec2& playerPosition);
  void render(GLSLShader& shader, const RENDER_TYPE renderType, GLuint globalMatricesUBO);
  void setTweakBar(TwBar * const bar);
  void showDebugInfo() const;
private:
  
  list<ChunkPtr> chunks;
  
  // Chunks That Are Being Processed In Separate Threads
  list<ChunkPtr> preparingChunks;
  
  // Used For Coloring Vertices
  glm::vec4 colorSet[2];
  
  // Level Of Detail Stuff **********
  
  // Is Level Of Detail Enabled
  bool lod = false;
  // Detail Levels For Determining How many vertices should given chunk have
  map<int, int> detailLevels;
  // Base Number Of Vertices
  int baseSideLength = (int)pow(2,6) + 1;// (int)pow(6,2);
  // Descention Rate Of Geometry
  float descentionRate = 0.5f;
  
  // ********************************
  
  
  GenData genData, prevGenData;
  
  //Minimum Value Should Be 2(1 is MainThread, and Minimum 1 ForRendering)
  int maxNumbOfThreads = 4;
  
  //Values from 0 to no. If ChunkRadius is set to 0 only 1 ChunkIsRendered 
  int chunkRadius = 4;
  
  //For Gui 
  bool isPerlin = true;
  
  // Checks If Threads Finished Their Work
  void checkChunks(GLSLShader& shader);
  
  // Checks If Settings Changed
  bool didSettingsChange();
  
  // Returns List of Chunks That Should Render assuming that passed position is Player Position
  list<ChunkData> getChunksForPosition(const glm::vec2& position) const;
  
  // Takes Players Position And Generates Required Chunks
  void generateRequiredChunks(const list<ChunkData>& requiredChunks);       
  
  // Deletes Unneeded Chunks
  void deleteUnneededChunks(const list<ChunkData>& requiredChunks);
  
  // Checks If Chunk Exist at Chunk Position
  bool doesChunkExists(const ChunkData& chunkData);
  
  // Deletes Chunk If Exists
  void deleteChunk(const glm::ivec2& chunkPosition);
  
  // Creates Chunk Object And Starts It's Thread
  void generateChunk(const ChunkData& chunkData);
  
  // Adds Surrounding Fields
  void addSurrounding(const glm::ivec2& position, list<glm::ivec2>& required) const;
  
  // Adds Fields in Square Radius ? Whaaat..
  void addFields(const glm::ivec2& position, list<ChunkData>& required, const int radius) const;
  
  // Adds Fields in Distance - Square
  void addFieldsInSquare(const glm::ivec2& position, list<ChunkData>& required, const int distance) const;
  
  // Gets Number Of Vertices(SideLength) Per DetailLevel
  int getNumbOfVertForDetailLevel(const int detailLevel);
  void recalculateDetailLevels();
};
