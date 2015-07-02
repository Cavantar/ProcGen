#pragma once

#include "Includes.h"
#include <jpb/Vector.h>

typedef vector<list<int>> AdjacencyList;

bool glCheckErrors();

#define  GL_CHECK_ERRORS if(glCheckErrors()) assert(0);

enum RENDER_TYPE{
	RT_POINTS,
	RT_LINES,
	RT_TRIANGLES
};

inline float interpFloat(const float x0, const float x1, const float t) {
  return (x1 - x0) * t + x0;
}

// TODO FIX ADJACENCY LIST FOR CYLINDER
AdjacencyList createGridAdjacencyList(const vector<Vec4f>& vertices, const glm::uvec2& dimensions, bool loop = false);
vector<glm::uvec2> createGridLineIndex(const int width, const int height);
vector<glm::uvec3> createGridTriangleIndex(const int width, const int height);

vector<Vec3f> getNormals(const vector<Vec4f>& vertices, vector<glm::uvec3>& faceIndex, AdjacencyList& adjacencyList = AdjacencyList());
vector<Vec3f> calculateFaceNormals(const vector<Vec4f>& vertices, vector<glm::uvec3>& faceIndex);

list<int> getTriangleIndexes(int srcIndex, vector<glm::uvec3>& faceIndex);
void reverseTriangleIndexes(vector<glm::uvec3>& triangles);
void addResersedTriangleIndexes(vector<glm::uvec3>& triangles);
void addIndexedQuad(vector<glm::uvec3>& triangles, glm::uvec4 indexes);

void translateVec4(vector<Vec4f>& vertices, Vec4f delta);
glm::vec2 getVec4Bounds(const vector<Vec4f>& vertices, const int dimension);

float getMaxVec4(const vector<Vec4f>& vertices, const int dimension);
float getMinVec4(const vector<Vec4f>& vertices, const int dimension);
float selectValVec4(const Vec4f& vector, const int dimension);
