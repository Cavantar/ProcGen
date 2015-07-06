#include "Net.h"

Net::Net(glm::uvec2& dimensions, vector<Vec4f>& vertices, GLSLShader& shader){
  prepareData(dimensions, vertices);
  copyToGfx(shader);
}
Net::~Net() {
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &bufferObject);
  glDeleteBuffers(1, &triangleIndexBuffer);
  glDeleteBuffers(1, &lineIndexBuffer);
}

void Net::prepareData(const glm::uvec2& dimensions, const vector<Vec4f>& vertices) {
  this->dimensions = dimensions;
  this->vertices = vertices;
  int rawDataSize = vertices.size() * 4 + vertices.size() * 3;
  
  // Pozycje i normalne
  rawData.resize(rawDataSize);
  memcpy(&rawData[0], &vertices[0], vertices.size() * sizeof(Vec4f));
  
  // Indeksy Linii
  lineIndexVec = createGridLineIndex(dimensions.x, dimensions.y);
  numbOfLines = lineIndexVec.size();
  
  // Indeksy Trójk±tów
  
  staticResourcesLock.lock();
  
  if(!trianglesIndexVecs.count(dimensions.x)) 
    trianglesIndexVecs[dimensions.x] = createGridTriangleIndex(dimensions.x, dimensions.y);
  numbOfTriangles = trianglesIndexVecs[dimensions.x].size();
  
  // Lista s¹siedztwa
  
  if(!adjacencyLists.count(dimensions.x))
    adjacencyLists[dimensions.x] = createGridAdjacencyList(vertices, dimensions);
  
  //static unsigned int tempTime = glutGet(GLUT_ELAPSED_TIME);
  
  //Normalne 
  normals = getNormals(vertices, trianglesIndexVecs[dimensions.x], adjacencyLists[dimensions.x]);
  memcpy(&rawData[vertices.size() * 4], &normals[0], normals.size() * sizeof(glm::vec3));
  
  staticResourcesLock.unlock();
  //cout << "Normals Took: " << glutGet(GLUT_ELAPSED_TIME) - tempTime << endl;
}

void Net::copyToGfx(GLSLShader& shader) {
  glGenBuffers(1, &lineIndexBuffer);
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIndexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numbOfLines * sizeof(glm::uvec2), &lineIndexVec[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  
  glGenBuffers(1, &triangleIndexBuffer);
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexBuffer);
  staticResourcesLock.lock();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numbOfTriangles * sizeof(glm::uvec3), &(trianglesIndexVecs[dimensions.x][0]), GL_STATIC_DRAW);
  staticResourcesLock.unlock();
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
  
  switch(renderType) {
  case RT_POINTS:
    glDrawArrays(GL_POINTS, 0, dimensions.x * dimensions.y);
    break;
  case RT_LINES:
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIndexBuffer);
    glDrawElements(GL_LINES, numbOfLines * 2, GL_UNSIGNED_INT, 0);
    break;
  case RT_TRIANGLES:
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIndexBuffer);
    //glDrawElements(GL_LINES, numbOfLines * 2, GL_UNSIGNED_INT, 0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexBuffer);
    glDrawElements(GL_TRIANGLES, numbOfTriangles * 3, GL_UNSIGNED_INT, 0);
    break;
  }
  glBindVertexArray(0);
}
glm::vec2 Net::getBounds(const int dimension) const {
  return getVec4Bounds(vertices, dimension);
}

map<int, vector<list<int>>> Net::adjacencyLists;
map<int, vector<glm::uvec3>> Net::trianglesIndexVecs;

mutex Net::staticResourcesLock;
