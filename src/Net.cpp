#include "Net.h"
#include <jpb/Profiler.h>

Net::Net(Vec2u& dimensions, vector<Vec4f>& vertices, GLSLShader& shader){
  prepareData(dimensions, vertices);
  copyToGfx(shader);
}
Net::~Net() {
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &bufferObject);
  glDeleteBuffers(1, &triangleIndexBuffer);
  glDeleteBuffers(1, &lineIndexBuffer);
}

void Net::prepareData(const Vec2u& dimensions, const vector<Vec4f>& newVertices) {
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

  // Lista s±siedztwa
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

void Net::prepareDataWithBounds(const Vec2u& internalDimensions, const vector<Vec4f>& newVertices) {

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

  // Lista s±siedztwa
  Profiler::get()->start("Adjacency");

  if(!adjacencyLists.count(totalDimensions.x))
    adjacencyLists[totalDimensions.x] = createGridAdjacencyList(totalDimensions);

  Profiler::get()->end("Adjacency");

  staticResourcesLock.unlock();

  numbOfTriangles = trianglesIndexVecs[dimensions.x].size();

  //Normalne

  Profiler::get()->start("Normals");
  vector<Vec3f> normalsWithBounds = getNormals(newVertices, trianglesIndexVecs[totalDimensions.x], adjacencyLists[totalDimensions.x]);

  // Wy³uskiwanie normalnych
  normals = getInsides(totalDimensions, normalsWithBounds);
  Profiler::get()->end("Normals");

  memcpy(&rawData[vertices.size() * 4], &normals[0], normals.size() * sizeof(Vec3f));
}

void Net::copyToGfx(GLSLShader& shader) {
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

void Net::render(const RENDER_TYPE renderType) const {
  glBindVertexArray(vao);

  static uint32 counter = 0;
  static int currentIndex = 40;
  uint32 period = 1000;
  counter++;

  // 49 First Line
  // 51 Second Line

  // if(counter > period)
  // {
  //   counter = counter % period;
  //   currentIndex++;
  //   //currentIndex = currentIndex % (dimensions.x * dimensions.y);
  //   currentIndex = currentIndex % numbOfLines;
  //   //currentIndex = ((dimensions.x * dimensions.y) / 2) + 1;
  //   //currentIndex = 30;
  //   std::cout << currentIndex << std::endl;
  //   //dimensions.showData();
  //   //std::cout << ((dimensions.x * dimensions.y) / 2)  <<std:: endl;
  //   //vertices[144].showData();
  // }

  switch(renderType) {
  case RT_POINTS:
    glDrawArrays(GL_POINTS, 0, (dimensions.x * dimensions.y));
    break;

  case RT_LINES:
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIndexBuffer);
    glDrawElements(GL_LINES, numbOfLines * 2, GL_UNSIGNED_INT, 0);
    break;

  case RT_TRIANGLES:
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIndexBuffer);
    // glDrawElements(GL_LINES, numbOfLines * 2, GL_UNSIGNED_INT, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexBuffer);
    glDrawElements(GL_TRIANGLES, numbOfTriangles * 3, GL_UNSIGNED_INT, 0);
    break;
  }
  glBindVertexArray(0);
}

Vec2f Net::getBounds(const int dimension) const {
  return getVec4Bounds(vertices, dimension);
}

map<int, vector<list<int>>> Net::adjacencyLists;
map<int, vector<Vec3u>> Net::trianglesIndexVecs;

mutex Net::staticResourcesLock;
