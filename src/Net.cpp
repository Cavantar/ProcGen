#include "Net.h"
#include <jpb/Profiler.h>

Net::Net(Vec2u& dimensions, std::vector<Vec4f>& vertices, GLSLShader& shader)
{
  prepareData(dimensions, vertices);
  copyToGfx(shader);
}

Net::~Net()
{
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &bufferObject);
  glDeleteBuffers(1, &triangleIndexBuffer);
  glDeleteBuffers(1, &lineIndexBuffer);
}

void Net::prepareData(const Vec2u& dimensions, const std::vector<Vec4f>& newVertices)
{
  this->dimensions = dimensions;
  vertices = newVertices;

  int rawDataSize = vertices.size() * 4 + vertices.size() * 3;

  // Pozycje i normalne
  rawData.resize(rawDataSize);
  memcpy(&rawData[0], &vertices[0], vertices.size() * sizeof(Vec4f));

  // Indeksy Linii
  Profiler::get()->start("Lines");
  lineIndexVec = createGridLineIndex(dimensions.x, dimensions.y);
  Profiler::get()->end("Lines");

  numbOfLines = lineIndexVec.size();

  // Indeksy Trójk±tów
  staticResourcesLock.lock();

  Profiler::get()->start("Triangles");
  if(!trianglesIndexVecs.count(dimensions.x))
    trianglesIndexVecs[dimensions.x] = createGridTriangleIndex(dimensions.x, dimensions.y);
  Profiler::get()->end("Triangles");

  // std::Lista s±siedztwa
  Profiler::get()->start("Adjacency");
  if(!adjacencyLists.count(dimensions.x))
    adjacencyLists[dimensions.x] = createGridAdjacencyList(dimensions);
  Profiler::get()->end("Adjacency");

  staticResourcesLock.unlock();

  numbOfTriangles = trianglesIndexVecs[dimensions.x].size();

  //Normalne
  Profiler::get()->start("Normals");
  normals = getNormals(vertices, trianglesIndexVecs[dimensions.x], adjacencyLists[dimensions.x]);
  Profiler::get()->end("Normals");

  memcpy(&rawData[vertices.size() * 4], &normals[0], normals.size() * sizeof(Vec3f));
}

void Net::prepareDataWithBounds(const Vec2u& internalDimensions, const std::vector<Vec4f>& newVertices)
{

  Vec2u totalDimensions = internalDimensions + Vec2u(2, 2);
  dimensions = internalDimensions;
  // Internal Vertices
  vertices = getInsides(totalDimensions, newVertices);

  int rawDataSize = vertices.size() * 4 + vertices.size() * 3;

  // Pozycje i normalne
  rawData.resize(rawDataSize);
  memcpy(&rawData[0], &vertices[0], vertices.size() * sizeof(Vec4f));

  // Indeksy Linii
  Profiler::get()->start("Lines");
  lineIndexVec = createGridLineIndex(dimensions.x, dimensions.y);
  Profiler::get()->end("Lines");

  numbOfLines = lineIndexVec.size();

  // Indeksy Trójk±tów
  staticResourcesLock.lock();

  // newVertices[0].showData();
  // newVertices[1 + totalDimensions.x].showData();
  // vertices[0].showData();
  // std::cout << "\n\n";
  Profiler::get()->start("Triangles");

  // Triangles For Copying and Rendering to Gfx
  if(!trianglesIndexVecs.count(dimensions.x))
    trianglesIndexVecs[dimensions.x] = createGridTriangleIndex(dimensions.x, dimensions.y);

  // Triangles For Normal Calculation
  if(!trianglesIndexVecs.count(totalDimensions.x))
    trianglesIndexVecs[totalDimensions.x] = createGridTriangleIndex(totalDimensions.x, totalDimensions.y);

  Profiler::get()->end("Triangles");

  // std::Lista s±siedztwa
  Profiler::get()->start("Adjacency");

  if(!adjacencyLists.count(totalDimensions.x))
    adjacencyLists[totalDimensions.x] = createGridAdjacencyList(totalDimensions);

  Profiler::get()->end("Adjacency");

  staticResourcesLock.unlock();

  numbOfTriangles = trianglesIndexVecs[dimensions.x].size();

  //Normalne

  Profiler::get()->start("Normals");
  std::vector<Vec3f> normalsWithBounds = getNormals(newVertices, trianglesIndexVecs[totalDimensions.x], adjacencyLists[totalDimensions.x]);

  // Wy³uskiwanie normalnych
  normals = getInsides(totalDimensions, normalsWithBounds);
  Profiler::get()->end("Normals");

  memcpy(&rawData[vertices.size() * 4], &normals[0], normals.size() * sizeof(Vec3f));
}

void
Net::copyToGfx(GLSLShader& shader)
{
  glGenBuffers(1, &lineIndexBuffer);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIndexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numbOfLines * sizeof(Vec2u), &lineIndexVec[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glGenBuffers(1, &triangleIndexBuffer);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numbOfTriangles * sizeof(Vec3u), &(trianglesIndexVecs[dimensions.x][0]), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glGenBuffers(1, &bufferObject);

  glBindBuffer(GL_ARRAY_BUFFER, bufferObject);
  glBufferData(GL_ARRAY_BUFFER, rawData.size() * 4, &rawData[0], GL_STREAM_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Tworzenie VAO
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, bufferObject);

  glEnableVertexAttribArray(shader["position"]);
  glVertexAttribPointer(shader["position"], 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(shader["normal"]);
  glVertexAttribPointer(shader["normal"], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(dimensions.x*dimensions.y * 4 * 4));
  glBindVertexArray(0);
  GL_CHECK_ERRORS;
}

void
Net::render(const RENDER_TYPE renderType) const
{
  glBindVertexArray(vao);

  static uint32 counter = 0;
  static int currentIndex = 40;
  uint32 period = 1000;
  counter++;

  switch(renderType) {
  case RT_POINTS:
    glDrawArrays(GL_POINTS, 0, (dimensions.x * dimensions.y));
    break;

  case RT_LINES:
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIndexBuffer);
    glDrawElements(GL_LINES, numbOfLines * 2, GL_UNSIGNED_INT, 0);
    break;

  case RT_TRIANGLES:

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexBuffer);
    glDrawElements(GL_TRIANGLES, numbOfTriangles * 3, GL_UNSIGNED_INT, 0);
    break;
  }
  glBindVertexArray(0);
}

Vec2f
Net::getBounds(const int dimension) const
{
  return getVec4Bounds(vertices, dimension);
}

void
Net::saveToObj(std::string filename) const
{
  std::ofstream file;
  file.open(filename.c_str(), std::ios::out);
  for(auto it = vertices.begin(); it != vertices.end(); it++)
  {
    const Vec4f& vec = *it;
    file << "v " << vec.x << " " << vec.y << " " << vec.z << std::endl;
  }

  const std::vector<Vec3u>& trianglesIndexVec = trianglesIndexVecs[dimensions.x];
  for(auto it = trianglesIndexVec.begin(); it != trianglesIndexVec.end(); it++)
  {
    const Vec3u& triangleIndex = *it;
    file << "f " << (triangleIndex.x + 1) << " " << (triangleIndex.y + 1) << " " << (triangleIndex.z + 1) << std::endl;
  }

  file.close();
}

std::map<int, std::vector<std::list<int>>> Net::adjacencyLists;
std::map<int, std::vector<Vec3u>> Net::trianglesIndexVecs;

std::mutex Net::staticResourcesLock;
