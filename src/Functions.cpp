#include "Functions.h"

bool glCheckErrors() {
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

AdjacencyList createGridAdjacencyList(const vector<Vec4f>& vertices, const glm::uvec2& dimensions, bool loop) {
  
  static unsigned int tempTime = glutGet(GLUT_ELAPSED_TIME);
  
  AdjacencyList adjacencyList;
  adjacencyList.resize(vertices.size());
  
  int trianglesPerLevel = (!loop ? (dimensions.x - 1) * 2 : dimensions.x * 2);
  //cout << "Creating Adjacency List For: " << vertices.size() << " Vertices" << endl;
  
  int index = 0;
  for(int y = 0; y < (int)dimensions.x; y++) {
    for(int x = 0; x < (int)dimensions.y; x++) {
      index = x + y * dimensions.y;
      
      
      // Lewa Kolumna
      if(x == 0) {
	// Je�eli nie jest ostatni wiersz
	if(y != dimensions.y - 1) {
	  // Dodajemy Z Poprzedniego Poziomu Tr�jk�t
	  adjacencyList[index].push_back(trianglesPerLevel * y);
	  adjacencyList[index].push_back(trianglesPerLevel * y + 1);
	}
	// Je�eli jest nie pierwszy albo ostatni
	if(y != 0 || y == dimensions.y - 1) {
	  adjacencyList[index].push_back(trianglesPerLevel * (y - 1) + 1);
	}
      }
      // Prawa Kolumna
      else if(x == dimensions.x - 1) {
	
	/*if(y != dimensions.y - 1){
	  adjacencyList[index].push_back(trianglesPerLevel - 2 + trianglesPerLevel * y);
	  adjacencyList[index].push_back(trianglesPerLevel - 2 + trianglesPerLevel * y + 1) ;
	  
	  }
	  if(y != 0) {
	  adjacencyList[index].push_back(trianglesPerLevel - 2 + trianglesPerLevel * (y - 1));
	  adjacencyList[index].push_back(trianglesPerLevel - 2 + trianglesPerLevel * (y - 1) + 1) ;
	  }*/
	
	// Je�eli nie jest ostatni wiersz
	if(y != dimensions.y - 1) {
	  // Dodajemy Z Poprzedniego Poziomu Tr�jk�t
	  adjacencyList[index].push_back(trianglesPerLevel * y + trianglesPerLevel - 2);
	  adjacencyList[index].push_back(trianglesPerLevel * y + 1 + trianglesPerLevel - 2);
	}
	// Je�eli jest nie pierwszy albo ostatni
	if(y != 0 || y == dimensions.y - 1) {
	  adjacencyList[index].push_back(trianglesPerLevel * (y - 1) + 1 + trianglesPerLevel - 2);
	}
	
			} // Wewn�trzne Kolumny
			else {
				// Pierwszy Rz�d
				if(y == 0) {
					adjacencyList[index].push_back((x - 1) * 2);
					adjacencyList[index].push_back((x - 1) * 2 + 1);
					adjacencyList[index].push_back((x - 1) * 2 + 2);
				}// Ostatni Rz�d
				else if(y == dimensions.y - 1) {
					adjacencyList[index].push_back((y - 1) *(dimensions.x - 1) * 2 + x * 2);
					adjacencyList[index].push_back((y - 1) *(dimensions.x - 1) * 2 + 1 + x * 2);
				}// �rodek
				else {
					// G�ra 
					adjacencyList[index].push_back((x - 1) * 2 + 1 + (y - 1) * trianglesPerLevel);
					adjacencyList[index].push_back((x - 1) * 2 + 1 + (y - 1) * trianglesPerLevel + 1);
					adjacencyList[index].push_back((x - 1) * 2 + 1 + (y - 1) * trianglesPerLevel + 2);
					// D�
					adjacencyList[index].push_back(y * trianglesPerLevel + (x * 2) - 2);
					adjacencyList[index].push_back(y * trianglesPerLevel + (x * 2) - 1);
					adjacencyList[index].push_back(y * trianglesPerLevel + (x * 2));
				}
			}
			//if(!((x + y * dimensions.y) % 1000))cout << "  Done: " << x + y * dimensions.y << " [ " << int((x + y * dimensions.y) / float(dimensions.x * dimensions.y) * 10000.0f) / 100 << " %] " << endl;
		}
	}
	//cout << "  Done: " << dimensions.x * dimensions.y << endl;
	//cout << "Finished \n\n";

	//cout << "Adjacency List Took: " << glutGet(GLUT_ELAPSED_TIME) - tempTime << endl;
  return adjacencyList;
}

vector<glm::uvec2> createGridLineIndex(const int width, const int height)
{
  const unsigned long int numbOfLines = 4 + 3 * (width - 2) + (3 + 2 * (width - 2)) * (height - 1);
  vector<glm::uvec2> indexVector;
  indexVector.resize(numbOfLines);
  
  int it = 0;
  for(int y = 0; y < height - 1; y++) {
    for(int x = 0; x < width - 1; x++) {
      // G�ra
      if(y == 0) indexVector[it++] = glm::uvec2(y * height + x, y * height + 1 + x);
      // Lewo
      if(x == 0) indexVector[it++] = glm::uvec2(y * height, (y + 1) * height);
      //Prawo 
      indexVector[it++] = glm::uvec2(y * height + x + 1, (y + 1) * height + x + 1);
      // D�
      indexVector[it++] = glm::uvec2((y + 1) * height + x, (y + 1) * height + 1 + x);
    }
  }
  return indexVector;
}

vector<glm::uvec3> createGridTriangleIndex(const int width, const int height)
{
  vector<glm::uvec3> triangles;
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

vector<Vec3f> getNormals(const vector<Vec4f>& vertices, vector<glm::uvec3>& faceIndex, AdjacencyList& adjacencyList)
{
  vector<Vec3f> normals;
  normals.resize(vertices.size());
  
  static unsigned int tempTime = glutGet(GLUT_ELAPSED_TIME);
  vector<Vec3f> faceNormals = calculateFaceNormals(vertices, faceIndex);
  
  Vec3f combinedNormal;
  tempTime = glutGet(GLUT_ELAPSED_TIME);
  
  int it = 0;
  for(auto i = vertices.begin(); i != vertices.end(); i++) {
    // Getting triangles that contain given vertex
    list<int>& indexes = adjacencyList[it];
    
    // Summing Up All The Normals Of Faces Connected To Vertex
    for(auto j = indexes.begin(); j != indexes.end(); j++)normals[it] += faceNormals[*j];
    normals[it] = Vec3f::normalize(normals[it]);
    it++;
  }
    
  return normals;
}

vector<Vec3f> calculateFaceNormals(const vector<Vec4f>& vertices, vector<glm::uvec3>& faceIndex)
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

list<int> getTriangleIndexes(int srcIndex, vector<glm::uvec3>& faceIndex)
{
  list<int> triIndex;
  for(auto i = faceIndex.begin(); i != faceIndex.end(); i++) {
    int index = distance(faceIndex.begin(), i);
    if(i->x == srcIndex || i->y == srcIndex || i->z == srcIndex) triIndex.push_back(index);
  }
  return triIndex;
}

void reverseTriangleIndexes(vector<glm::uvec3>& triangles)
{
  unsigned int temp;
  for(auto i = triangles.begin(); i != triangles.end(); i++) {
    temp = i->x;
    i->x = i->z;
    i->z = temp;
  }
}

void addResersedTriangleIndexes(vector<glm::uvec3>& triangles)
{
  int orgSize = triangles.size() * 2;
  triangles.resize(orgSize * 2);
  for(int i = 0; i < orgSize ; i++) {
    glm::uvec3& currVec = triangles[i];
    triangles[i + orgSize] = glm::uvec3(currVec.z, currVec.y, currVec.x);
  }
}

void addIndexedQuad(vector<glm::uvec3>& triangles, glm::uvec4 indexes)
{
  triangles.push_back(glm::uvec3(indexes.x, indexes.y, indexes.z));
  triangles.push_back(glm::uvec3(indexes.x, indexes.z, indexes.w));
}

void translateVec4(vector<Vec4f>& vertices, Vec4f delta)
{
  for(auto i = vertices.begin(); i != vertices.end(); i++) {
    (*i) += delta;
  }
}

glm::vec2 getVec4Bounds(const vector<Vec4f>& vertices, const int dimension)
{
  glm::vec2 bounds;
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

float getMaxVec4(const vector<Vec4f>& vertices, const int dimension)
{
  float max = 0;
  for(auto i = vertices.begin(); i != vertices.end(); i++) {
    float temp = selectValVec4(*i, dimension);
    if(i == vertices.begin()) max = temp;
    else if(temp > max)  max = temp;
  }
  return max;
}

float getMinVec4(const vector<Vec4f>& vertices, const int dimension)
{
  float min = 0;
  for(auto i = vertices.begin(); i != vertices.end(); i++) {
    float temp = selectValVec4(*i, dimension);
    if(i == vertices.begin()) min = temp;
    else if(temp < min)  min = temp;
  }
  return min;
}

float selectValVec4(const Vec4f& vector, const int dimension)
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
