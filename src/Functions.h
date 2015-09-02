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
AdjacencyList createGridAdjacencyList(const Vec2u& dimensions, bool loop = false);
vector<Vec2u> createGridLineIndex(const int width, const int height);
vector<Vec3u> createGridTriangleIndex(const int width, const int height);

vector<Vec4f> getInsides(const Vec2u& totalDimensions, const vector<Vec4f>& vertices);
vector<Vec3f> getInsides(const Vec2u& totalDimensions, const vector<Vec3f>& vertices);

vector<Vec3f> getNormals(const vector<Vec4f>& vertices, vector<Vec3u>& faceIndex, AdjacencyList& adjacencyList = AdjacencyList());
vector<Vec3f> calculateFaceNormals(const vector<Vec4f>& vertices, vector<Vec3u>& faceIndex);

list<int> getTriangleIndexes(int srcIndex, vector<Vec3u>& faceIndex);
void reverseTriangleIndexes(vector<Vec3u>& triangles);
void addResersedTriangleIndexes(vector<Vec3u>& triangles);
void addIndexedQuad(vector<Vec3u>& triangles, Vec4u indexes);

void translateVec4(vector<Vec4f>& vertices, Vec4f delta);
Vec2f getVec4Bounds(const vector<Vec4f>& vertices, const int dimension);

float getMaxVec4(const vector<Vec4f>& vertices, const int dimension);
float getMinVec4(const vector<Vec4f>& vertices, const int dimension);
float selectValVec4(const Vec4f& vector, const int dimension);
