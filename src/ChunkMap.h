#pragma once
#include <unordered_map>
#include "Includes.h"
#include "GLSLShader.h"
#include "Chunk.h"
#include "Camera.h"

typedef std::shared_ptr<Chunk> ChunkPtr ;

// Used Mostly For Requesting Chunks Based On The Criteria Present as Fields
class ChunkData{
public:
  Vec2i position;
  int detailLevel;
  int distanceFromCamera;

  ChunkData(const Vec2i position, const int detailLevel, int distanceFromCamera = 0):
    position(position), detailLevel(detailLevel), distanceFromCamera(distanceFromCamera) {}
};

// For Sorting Which chunks to render first
bool compareChunkData(const ChunkData& chunkData1, const ChunkData& chunkData2);

class MapGenData;
class ChunkMap {
public:
  ChunkMap();
  ~ChunkMap();
  void update(GLSLShader& shader, Vec2f& cameraPosition);
  void render(GLSLShader& shader, const RENDER_TYPE renderType, GLuint globalMatricesUBO, const CameraData& cameraData);

  void setTweakBar(TwBar * const bar);
  void showDebugInfo() const;

  void cleanUp(GLSLShader& shader);

  void setMapGenData(const MapGenData* mapGenData) { this->mapGenData = mapGenData; recalculateDetailLevels();}
  const Vec4f& getFogColor() const { return fogColor; }

  void regenerate() { shouldRegenerate = true;}
private:
  const MapGenData* mapGenData;

  // Presentation Stuff
  // --------------
  // cString For Expression in Ant Tweak Bar
  TwBar* bar;

  // Filename to save obj buffer
  char objFilenameAnt[255];

  bool shouldSaveGeometry = false;
  bool shouldRegenerate = false;

  // --------------
  // Generated Chunks
  std::list<ChunkPtr> chunks;

  // Chunks That Are Being Processed In Separate Threads
  std::list<ChunkPtr> preparingChunks;

  int32 chunkExportCount = 8;

  // Level Of Detail Stuff **********

  // Is Level Of Detail Enabled
  bool lod = false;
  // Detail Levels For Determining How many vertices should given chunk have
  std::map<int, int> detailLevels;
  // Descention Rate Of Geometry
  float descentionRate = 0.5f;

  // ********************************

  // Render Behind
  bool renderBehind = false;
  bool turnOffNormals = false;
  bool clampFog = true;
  bool exportBoundsOn = false;

  Vec2f fogBounds = Vec2f(300, 900);
  Vec4f fogColor = Vec4f(0.5f, 0.5f, 0.5f, 0.5f);

  //Minimum Value Should Be 2(1 is MainThread and Rendering, and Minimum 1 For Generation)
  int maxNumbOfThreads = 4;

  //Values from 1.
  int chunkRadius = 8;

  // Checks If Threads Finished Their Work
  void processThreads(GLSLShader& shader);

  // Returns std::List of Chunks That Should Render assuming that passed position is Player Position
  std::list<ChunkData> getChunksForPosition(const Vec2f& position) const;

  // Takes Players Position And Generates Required Chunks
  void generateRequiredChunks(const std::list<ChunkData>& requiredChunks);

  // Regenerates Chunks
  void regenerateChunks();

  // Deletes Unneeded Chunks
  void deleteUnneededChunks(const std::list<ChunkData>& requiredChunks);

  // Checks If Chunk Exist at Chunk Position
  bool doesChunkExists(const ChunkData& chunkData);

  // Deletes Chunk If Exists
  void deleteChunk(const Vec2i& chunkPosition);

  // Creates Chunk Object And Starts It's Thread
  void generateChunk(const ChunkData& chunkData);

  // Adds Surrounding Fields
  void addSurrounding(const Vec2i& position, std::list<Vec2i>& required) const;

  // Adds Fields in Square Radius ? Whaaat..
  void addFields(const Vec2i& position, std::list<ChunkData>& required, const int radius) const;

  // Adds Fields in Distance - Square
  void addFieldsInSquare(const Vec2i& position, std::list<ChunkData>& required, const int distance) const;

  // Gets Number Of Vertices(SideLength) Per DetailLevel
  int getNumbOfVertForDetailLevel(const int detailLevel);

  bool shouldChunkBeRendered(const ChunkPtr chunk, const CameraData& cameraData) const;

  void recalculateDetailLevels();
};

struct ListColor{
  Vec3f color;
  real32 startValue;
  bool shouldDelete;

  // Used for inserting colors above the selected one.
  bool insertColor;
  int32 indexOnTheList;

  bool operator==(const ListColor& listColor) const
  {
    bool result = listColor.color == color &&
      listColor.startValue == startValue &&
      listColor.indexOnTheList == indexOnTheList;

    return result;
  }
};

typedef std::list<ListColor> ColorList;

class MapGenData {
public:
  friend class ChunkMap;
  friend class TextureModule;

  void initialize(TwBar* bar);
  void update(TwBar * const bar);
  std::string getExpressionText() const {return currentExpAnt;}
  void setExpressionText(const std::string& text);
  void setSettingsFilename(const std::string& text);

private:
  TwType noiseType;

  char expressionAnt[255];
  char currentExpAnt[255];
  char filenameAnt[255];

  std::string currentExpression;
  std::string previousExpression;

  bool shouldLoadSettings = false;
  bool shouldSaveSettings = false;

  bool renderExpression = true;
  bool prevRenderExpression = true;

  int32 currentMapIndex = 1;
  int32 prevMapIndex = 1;

  int32 resolution = (int)pow(2,6) + 1;
  int32 prevResolution = resolution;

  GenDataMap genDataMap;
  GenData genData, prevGenData;

  bool shouldRegenerate = false;
  bool colorChanged = false;

  int32 maxColorIndex = 1;
  ColorList colorList;
  ColorList prevColorList;
  bool addColor = false;

  void saveState(const std::string& filename);

  // Because tweak bar is part of state.
  void loadState(TwBar * const bar, const std::string& filename);

  void addListColor(TwBar * const bar, ListColor listColor);
  void deleteListColor(TwBar * const bar, int32 colorIndex);
  void updateColors(TwBar * const bar);

  // Filtered by current expression
  // if renderExpression is false it works accordingly
  GenDataMap getFilteredGenDataMap(const std::string& expression) const;
};
