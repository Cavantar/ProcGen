#include <jpb/Profiler.h>
#include "Functions.h"

bool glCheckErrors()
{
  GLenum error = glGetError();
  if(error != GL_NO_ERROR) {
    while(error != GL_NO_ERROR) {
      switch(error) {
      case GL_INVALID_ENUM:
	std::cout << "InvalidEnum\n";
	break;
      case GL_INVALID_VALUE:
	std::cout << "InvalidValue\n";
	break;
      case GL_INVALID_OPERATION:
	std::cout << "InvalidOperation\n";
	break;
      case GL_STACK_OVERFLOW:
	std::cout << "StackOverflow\n";
	break;
      case GL_STACK_UNDERFLOW:
	std::cout << "StackUnderOverflow\n";
	break;
      case GL_OUT_OF_MEMORY:
	std::cout << "OutOfMemory\n";
	break;
      case GL_TABLE_TOO_LARGE:
	std::cout << "TableTooLarge\n";
	break;
      default:
	std::cout << " WTFF ??? \n";
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

std::vector<Vec2u>
createGridLineIndex(const int width, const int height)
{
  const unsigned long int numbOfLines = 4 + 3 * (width - 2) + (3 + 2 * (width - 2)) * (height - 1);
  std::vector<Vec2u> indexVector;
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

std::vector<Vec3u> createGridTriangleIndex(const int width, const int height)
{
  std::vector<Vec3u> triangles;
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

std::vector<Vec4f>
getInsides(const Vec2u& totalDimensions, const std::vector<Vec4f>& vertices)
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

std::vector<Vec3f>
getInsides(const Vec2u& totalDimensions, const std::vector<Vec3f>& vertices)
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

std::vector<Vec3f>
getNormals(const std::vector<Vec4f>& vertices, std::vector<Vec3u>& faceIndex, AdjacencyList& adjacencyList)
{
  std::vector<Vec3f> normals;
  normals.resize(vertices.size());

  Profiler::get()->start("FaceNormals");
  std::vector<Vec3f> faceNormals = calculateFaceNormals(vertices, faceIndex);
  Profiler::get()->end("FaceNormals");

  Vec3f combinedNormal;

  int it = 0;
  for(auto i = vertices.begin(); i != vertices.end(); i++)
  {

    // Getting triangles that contain given vertex
    std::list<int>& indexes = adjacencyList[it];

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

std::vector<Vec3f>
calculateFaceNormals(const std::vector<Vec4f>& vertices, std::vector<Vec3u>& faceIndex)
{
  std::vector<Vec3f> faceNormals;
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

std::list<int>
getTriangleIndexes(int srcIndex, std::vector<Vec3u>& faceIndex)
{
  std::list<int> triIndex;
  for(auto i = faceIndex.begin(); i != faceIndex.end(); i++) {
    int index = distance(faceIndex.begin(), i);
    if(i->x == srcIndex || i->y == srcIndex || i->z == srcIndex) triIndex.push_back(index);
  }
  return triIndex;
}

void
reverseTriangleIndexes(std::vector<Vec3u>& triangles)
{
  unsigned int temp;
  for(auto i = triangles.begin(); i != triangles.end(); i++) {
    temp = i->x;
    i->x = i->z;
    i->z = temp;
  }
}

void
addResersedTriangleIndexes(std::vector<Vec3u>& triangles)
{
  int orgSize = triangles.size() * 2;
  triangles.resize(orgSize * 2);
  for(int i = 0; i < orgSize ; i++) {
    Vec3u& currVec = triangles[i];
    triangles[i + orgSize] = Vec3u(currVec.z, currVec.y, currVec.x);
  }
}

void
addIndexedQuad(std::vector<Vec3u>& triangles, Vec4u indexes)
{
  triangles.push_back(Vec3u(indexes.x, indexes.y, indexes.z));
  triangles.push_back(Vec3u(indexes.x, indexes.z, indexes.w));
}

void
translateVec4(std::vector<Vec4f>& vertices, Vec4f delta)
{
  for(auto i = vertices.begin(); i != vertices.end(); i++) {
    (*i) += delta;
  }
}

Vec2f
getVec4Bounds(const std::vector<Vec4f>& vertices, const int dimension)
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
getMaxVec4(const std::vector<Vec4f>& vertices, const int dimension)
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
getMinVec4(const std::vector<Vec4f>& vertices, const int dimension)
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
    std::cout << "YOLO\n";
  }
  return 0;
}

void
saveTexture(const std::vector<Vec3f>& colors, const Vec2u& dimensions, std::string filename)
{
  BITMAPFILEHEADER bmfh;
  BITMAPINFOHEADER info;

  memset ( &bmfh, 0, sizeof (BITMAPFILEHEADER ) );
  memset ( &info, 0, sizeof (BITMAPINFOHEADER ) );
  //Next we fill the file header with data:

  long paddedSize = dimensions.x * dimensions.y * 3;

  bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
  bmfh.bfReserved1 = 0;
  bmfh.bfReserved2 = 0;
  bmfh.bfSize = sizeof(BITMAPFILEHEADER) +
    sizeof(BITMAPINFOHEADER) + paddedSize;

  bmfh.bfOffBits = 0x36;

  info.biSize = sizeof(BITMAPINFOHEADER);
  info.biWidth = dimensions.x;
  info.biHeight = dimensions.y;
  info.biPlanes = 1;
  info.biBitCount = 24;
  info.biCompression = BI_RGB;
  info.biSizeImage = 0;
  info.biXPelsPerMeter = 0x0ec4;
  info.biYPelsPerMeter = 0x0ec4;
  info.biClrUsed = 0;
  info.biClrImportant = 0;

  std::ofstream file;
  file.open(filename.c_str(), std::ios::out | std::ios::binary);
  file.write((char*)&bmfh, sizeof(BITMAPFILEHEADER));
  file.write((char*)&info, sizeof(BITMAPINFOHEADER));

  for(auto it = colors.begin(); it != colors.end(); it++)
  {
    const Vec3f& color = *it;

    uint32 red = uint8(color.x * 255);
    uint32 green = uint8(color.y * 255);
    uint32 blue = uint8(color.z * 255);

    uint32 resultColor = blue | (green << 8) | (red << 16);

    file.write((char*)&resultColor, 3);
  }

  file.close();
}
