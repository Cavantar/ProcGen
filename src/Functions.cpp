#include <jpb/Profiler.h>
#include "Functions.h"

bool glCheckErrors()
{
  GLenum error = glGetError();
  if(error != GL_NO_ERROR) {
    while(error != GL_NO_ERROR) {
      switch(error) {
      case GL_INVALID_ENUM:
	cout << "InvalidEnum\n";
	break;
      case GL_INVALID_VALUE:
	cout << "InvalidValue\n";
	break;
      case GL_INVALID_OPERATION:
	cout << "InvalidOperation\n";
	break;
      case GL_STACK_OVERFLOW:
	cout << "StackOverflow\n";
	break;
      case GL_STACK_UNDERFLOW:
	cout << "StackUnderOverflow\n";
	break;
      case GL_OUT_OF_MEMORY:
	cout << "OutOfMemory\n";
	break;
      case GL_TABLE_TOO_LARGE:
	cout << "TableTooLarge\n";
	break;
      default:
	cout << " WTFF ??? \n";
      }
      error = glGetError();
      return true;
    }
    return true;
  }
  return false;
}

AdjacencyList
createGridAdjacencyList(const Vec2u& dimensions, bool loop)
{
  AdjacencyList adjacencyList;
  adjacencyList.resize(dimensions.x * dimensions.y);
  
  int trianglesPerLevel = (!loop ? (dimensions.x - 1) * 2 : dimensions.x * 2);
  
  int index = 0;
  for(int y = 0; y < (int)dimensions.x; y++) {
    for(int x = 0; x < (int)dimensions.y; x++) {
      
      index = x + y * dimensions.y;
      
      // Lewa Kolumna
      if(x == 0) {
	// Je¿eli nie jest ostatni wiersz
	if(y != dimensions.y - 1) {
	  // Dodajemy Z Poprzedniego Poziomu Trójk¹t
	  adjacencyList[index].push_back(trianglesPerLevel * y);
	  adjacencyList[index].push_back(trianglesPerLevel * y + 1);
	}
	// Je¿eli jest nie pierwszy albo ostatni
	if(y != 0 || y == dimensions.y - 1) {
	  adjacencyList[index].push_back(trianglesPerLevel * (y - 1) + 1);
	}
      }
      // Prawa Kolumna
      else if(x == dimensions.x - 1) {
	// Je¿eli nie jest ostatni wiersz
	if(y != dimensions.y - 1) {
	  // Dodajemy Z Poprzedniego Poziomu Trójk¹t
	  adjacencyList[index].push_back(trianglesPerLevel * y + trianglesPerLevel - 2);
	  adjacencyList[index].push_back(trianglesPerLevel * y + 1 + trianglesPerLevel - 2);
	}
	// Je¿eli jest nie pierwszy albo ostatni
	if(y != 0 || y == dimensions.y - 1) {
	  adjacencyList[index].push_back(trianglesPerLevel * (y - 1) + 1 + trianglesPerLevel - 2);
	}
      } // Wewnêtrzne Kolumny
      else {
	// Pierwszy Rz¹d
	if(y == 0) {
	  adjacencyList[index].push_back((x - 1) * 2);
	  adjacencyList[index].push_back((x - 1) * 2 + 1);
	  adjacencyList[index].push_back((x - 1) * 2 + 2);
	}// Ostatni Rz¹d
	else if(y == dimensions.y - 1) {
	  adjacencyList[index].push_back((y - 1) *(dimensions.x - 1) * 2 + x * 2);
	  adjacencyList[index].push_back((y - 1) *(dimensions.x - 1) * 2 + 1 + x * 2);
	}// Œrodek
	else {
	  // Góra 
	  adjacencyList[index].push_back((x - 1) * 2 + 1 + (y - 1) * trianglesPerLevel);
	  adjacencyList[index].push_back((x - 1) * 2 + 1 + (y - 1) * trianglesPerLevel + 1);
	  adjacencyList[index].push_back((x - 1) * 2 + 1 + (y - 1) * trianglesPerLevel + 2);
	  // Dó³
	  adjacencyList[index].push_back(y * trianglesPerLevel + (x * 2) - 2);
	  adjacencyList[index].push_back(y * trianglesPerLevel + (x * 2) - 1);
	  adjacencyList[index].push_back(y * trianglesPerLevel + (x * 2));
	}
      }
    }
  }
  
  return adjacencyList;
}

vector<Vec2u>
createGridLineIndex(const int width, const int height)
{
  const unsigned long int numbOfLines = 4 + 3 * (width - 2) + (3 + 2 * (width - 2)) * (height - 1);
  vector<Vec2u> indexVector;
  indexVector.resize(numbOfLines);
  
  int it = 0;
  for(int y = 0; y < height - 1; y++) {
    for(int x = 0; x < width - 1; x++) {
      // Góra
      if(y == 0) indexVector[it++] = Vec2u(y * height + x, y * height + 1 + x);
      // Lewo
      if(x == 0) indexVector[it++] = Vec2u(y * height, (y + 1) * height);
      //Prawo 
      indexVector[it++] = Vec2u(y * height + x + 1, (y + 1) * height + x + 1);
      // Dó³
      indexVector[it++] = Vec2u((y + 1) * height + x, (y + 1) * height + 1 + x);
    }
  }
  return indexVector;
}

vector<Vec3u> createGridTriangleIndex(const int width, const int height)
{
  vector<Vec3u> triangles;
  triangles.resize((width - 1) * (height - 1) * 2);
  
  int it = 0;
  for(int y = 0; y < width - 1; y++) {
    for(int x = 0; x < height - 1; x++) {
      triangles[it].x = x + y * width;
      triangles[it].y = (x + 1) % width + y * width;
      triangles[it].z = (x + 1) % width + (y + 1) * width;
      
      it++;
      
      triangles[it].x = x + y * width;
      triangles[it].y = (x + 1) % width + (y + 1) * width;
      triangles[it].z = x + (y + 1) * width;
      
      it++;
    }
  }
  return triangles;
}

vector<Vec4f>
getInsides(const Vec2u& totalDimensions, const vector<Vec4f>& vertices)
{
  std::vector<Vec4f> result;
  result.resize((totalDimensions.x - 2) * (totalDimensions.y - 2));
  for(int y = 1; y < totalDimensions.y-1; y++)
  {
    for(int x = 1; x < totalDimensions.x-1; x++)
    {
      int srcIndex = x + (y * totalDimensions.x);
      int resultIndex = (x-1) + ((y-1) * (totalDimensions.x - 2));
      result[resultIndex] = vertices[srcIndex];
    }
  }
  return result;
}

vector<Vec3f>
getInsides(const Vec2u& totalDimensions, const vector<Vec3f>& vertices)
{
  std::vector<Vec3f> result;
  result.resize((totalDimensions.x - 2) * (totalDimensions.y - 2));
  for(int y = 1; y < totalDimensions.y-1; y++)
  {
    for(int x = 1; x < totalDimensions.x-1; x++)
    {
      int srcIndex = x + (y * totalDimensions.x);
      int resultIndex = (x-1) + ((y-1) * (totalDimensions.x - 2));
      result[resultIndex] = vertices[srcIndex];
    }
  }
  return result;
}

vector<Vec3f>
getNormals(const vector<Vec4f>& vertices, vector<Vec3u>& faceIndex, AdjacencyList& adjacencyList)
{
  vector<Vec3f> normals;
  normals.resize(vertices.size());

  Profiler::get()->start("FaceNormals");
  vector<Vec3f> faceNormals = calculateFaceNormals(vertices, faceIndex);
  Profiler::get()->end("FaceNormals");

  Vec3f combinedNormal;
  
  int it = 0;
  for(auto i = vertices.begin(); i != vertices.end(); i++)
  {
    
    // Getting triangles that contain given vertex
    list<int>& indexes = adjacencyList[it];
    
    // Summing Up All The Normals Of Faces Connected To Vertex
    //Vec3f& normal = normals[it];
    for(auto j = indexes.begin(); j != indexes.end(); j++)
    {
      normals[it] += faceNormals[*j];
    }
    
    normals[it] = Vec3f::normalize(normals[it]);
    it++;
  }
  
  return normals;
}

vector<Vec3f>
calculateFaceNormals(const vector<Vec4f>& vertices, vector<Vec3u>& faceIndex)
{
  vector<Vec3f> faceNormals;
  faceNormals.resize(faceIndex.size());
  
  int it = 0;
  for(auto i = faceIndex.begin(); i != faceIndex.end(); i++) {
    
    Vec3f firstParameter(vertices[i->x].x - vertices[i->y].x,
			 vertices[i->x].y - vertices[i->y].y,
			 vertices[i->x].z - vertices[i->y].z);
    
    Vec3f secondParameter(vertices[i->z].x - vertices[i->y].x,
			  vertices[i->z].y - vertices[i->y].y,
			  vertices[i->z].z - vertices[i->y].z);
    
    faceNormals[it++] = Vec3f::cross(firstParameter, secondParameter);
  }
  
  return faceNormals;
}

list<int>
getTriangleIndexes(int srcIndex, vector<Vec3u>& faceIndex)
{
  list<int> triIndex;
  for(auto i = faceIndex.begin(); i != faceIndex.end(); i++) {
    int index = distance(faceIndex.begin(), i);
    if(i->x == srcIndex || i->y == srcIndex || i->z == srcIndex) triIndex.push_back(index);
  }
  return triIndex;
}

void
reverseTriangleIndexes(vector<Vec3u>& triangles)
{
  unsigned int temp;
  for(auto i = triangles.begin(); i != triangles.end(); i++) {
    temp = i->x;
    i->x = i->z;
    i->z = temp;
  }
}

void
addResersedTriangleIndexes(vector<Vec3u>& triangles)
{
  int orgSize = triangles.size() * 2;
  triangles.resize(orgSize * 2);
  for(int i = 0; i < orgSize ; i++) {
    Vec3u& currVec = triangles[i];
    triangles[i + orgSize] = Vec3u(currVec.z, currVec.y, currVec.x);
  }
}

void
addIndexedQuad(vector<Vec3u>& triangles, Vec4u indexes)
{
  triangles.push_back(Vec3u(indexes.x, indexes.y, indexes.z));
  triangles.push_back(Vec3u(indexes.x, indexes.z, indexes.w));
}

void
translateVec4(vector<Vec4f>& vertices, Vec4f delta)
{
  for(auto i = vertices.begin(); i != vertices.end(); i++) {
    (*i) += delta;
  }
}

Vec2f
getVec4Bounds(const vector<Vec4f>& vertices, const int dimension)
{
  Vec2f bounds;
  float temp;
  for(auto i = vertices.begin(); i != vertices.end(); i++) {
    temp = selectValVec4(*i, dimension);
    if(i != vertices.begin()) {
      if(temp < bounds.x) bounds.x = temp;
      if(temp > bounds.y) bounds.y = temp;
    }
    else {
      bounds.x = temp;
      bounds.y = temp;
    }
  }
  return bounds;
}

float
getMaxVec4(const vector<Vec4f>& vertices, const int dimension)
{
  float max = 0;
  for(auto i = vertices.begin(); i != vertices.end(); i++) {
    float temp = selectValVec4(*i, dimension);
    if(i == vertices.begin()) max = temp;
    else if(temp > max)  max = temp;
  }
  return max;
}

float
getMinVec4(const vector<Vec4f>& vertices, const int dimension)
{
  float min = 0;
  for(auto i = vertices.begin(); i != vertices.end(); i++) {
    float temp = selectValVec4(*i, dimension);
    if(i == vertices.begin()) min = temp;
    else if(temp < min)  min = temp;
  }
  return min;
}

float
selectValVec4(const Vec4f& vector, const int dimension)
{
  switch(dimension){
  case 1: return vector.x;
  case 2: return vector.y;
  case 3: return vector.z;
  case 4: return vector.w;
    
  default:
    cout << "YOLO\n";
  }
  return 0;
}
