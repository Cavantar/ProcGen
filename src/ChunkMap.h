#pragma once
#include <unordered_map>
#include "Includes.h"
#include "Chunk.h"

typedef std::shared_ptr<Chunk> ChunkPtr ;

// Used Mostly For Requesting Chunks Based On The Criteria Present as Fields
class ChunkData{
public:
  glm::ivec2 position;
  int detailLevel;
  int distanceFromCamera;

  ChunkData(const glm::ivec2 position, const int detailLevel, int distanceFromCamera = 0):
    position(position), detailLevel(detailLevel), distanceFromCamera(distanceFromCamera) {}
};

// For Sorting Which chunks to render first
bool compareChunkData(const ChunkData& chunkData1, const ChunkData& chunkData2);

class ChunkMap {
public:
  ChunkMap();
  ~ChunkMap();
  void update(GLSLShader& shader, glm::vec2& playerPosition);
  void render(GLSLShader& shader, const RENDER_TYPE renderType, GLuint globalMatricesUBO, const CameraData& cameraData);

  void setTweakBar(TwBar * const bar);
  void showDebugInfo() const;

  void cleanUp(GLSLShader& shader);
private:

  // Presentation Stuff
  // --------------

  // cString For Expression in Ant Tweak Bar
  char expressionAnt[255];
  char currentExpAnt[255];
  char filenameAnt[255];

  TwBar* bar;
  TwType noiseType;

  std::string currentExpression;
  std::string previousExpression;

  int currentMapIndex = 1;
  int prevMapIndex = 1;

  bool renderExpression = true;
  bool prevRenderExpression = true;

  bool shouldLoadSettings = false;
  bool shouldSaveSettings = false;
  bool shouldSaveGeometry = false;

  GenDataMap genDataMap;
  bool shouldRegenerate = false;

  // --------------
  std::list<ChunkPtr> chunks;

  // Chunks That Are Being Processed In Separate Threads
  std::list<ChunkPtr> preparingChunks;

  // Used For Coloring Vertices
  glm::vec4 colorSet[2];

  // Level Of Detail Stuff **********

  // Is Level Of Detail Enabled
  bool lod = false;
  // Detail Levels For Determining How many vertices should given chunk have
  std::map<int, int> detailLevels;
  // Base Number Of Vertices
  int baseSideLength = (int)pow(2,6) + 1;// (int)pow(6,2);
  //int baseSideLength = (int)pow(2,3) + 1;// (int)pow(6,2);
  // Descention Rate Of Geometry
  float descentionRate = 0.5f;

  // ********************************

  // Render Behind

  bool renderBehind = false;
  GenData genData, prevGenData;

  //Minimum Value Should Be 2(1 is MainThread, and Minimum 1 ForRendering)
  int maxNumbOfThreads = 4;

  //Values from 0 to no. If ChunkRadius is set to 0 only 1 ChunkIsRendered
  int chunkRadius = 8;

  // Checks If Threads Finished Their Work
  void processThreads(GLSLShader& shader);

  // Returns std::List of Chunks That Should Render assuming that passed position is Player Position
  std::list<ChunkData> getChunksForPosition(const glm::vec2& position) const;

  // Takes Players Position And Generates Required Chunks
  void generateRequiredChunks(const std::list<ChunkData>& requiredChunks);

  // Regenerates Chunks
  void regenerateChunks();

  // Deletes Unneeded Chunks
  void deleteUnneededChunks(const std::list<ChunkData>& requiredChunks);

  // Checks If Chunk Exist at Chunk Position
  bool doesChunkExists(const ChunkData& chunkData);

  // Deletes Chunk If Exists
  void deleteChunk(const glm::ivec2& chunkPosition);

  // Creates Chunk Object And Starts It's Thread
  void generateChunk(const ChunkData& chunkData);

  // Adds Surrounding Fields
  void addSurrounding(const glm::ivec2& position, std::list<glm::ivec2>& required) const;

  // Adds Fields in Square Radius ? Whaaat..
  void addFields(const glm::ivec2& position, std::list<ChunkData>& required, const int radius) const;

  // Adds Fields in Distance - Square
  void addFieldsInSquare(const glm::ivec2& position, std::list<ChunkData>& required, const int distance) const;

  // Gets Number Of Vertices(SideLength) Per DetailLevel
  int getNumbOfVertForDetailLevel(const int detailLevel);

  bool shouldChunkBeRendered(const ChunkPtr chunk, const CameraData& cameraData) const;

  void recalculateDetailLevels();

  void saveState(const std::string& filename);
  void loadState(const std::string& filename);
};
