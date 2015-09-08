#pragma once

#include "Includes.h"
#include <jpb/Vector.h>

typedef std::vector<std::list<int>> AdjacencyList;

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
AdjacencyList
createGridAdjacencyList(const Vec2u& dimensions, bool loop = false);
std::vector<Vec2u>
createGridLineIndex(const int width, const int height);
std::vector<Vec3u>
createGridTriangleIndex(const int width, const int height);

std::vector<Vec4f>
getInsides(const Vec2u& totalDimensions, const std::vector<Vec4f>& vertices);
std::vector<Vec3f>
getInsides(const Vec2u& totalDimensions, const std::vector<Vec3f>& vertices);

std::vector<Vec3f>
getNormals(const std::vector<Vec4f>& vertices, std::vector<Vec3u>& faceIndex, AdjacencyList& adjacencyList = AdjacencyList());

std::vector<Vec3f>
calculateFaceNormals(const std::vector<Vec4f>& vertices, std::vector<Vec3u>& faceIndex);

std::list<int>

getTriangleIndexes(int srcIndex, std::vector<Vec3u>& faceIndex);
void
reverseTriangleIndexes(std::vector<Vec3u>& triangles);

void
addResersedTriangleIndexes(std::vector<Vec3u>& triangles);
void
addIndexedQuad(std::vector<Vec3u>& triangles, Vec4u indexes);

void
translateVec4(std::vector<Vec4f>& vertices, Vec4f delta);
Vec2f
getVec4Bounds(const std::vector<Vec4f>& vertices, const int dimension);

float
getMaxVec4(const std::vector<Vec4f>& vertices, const int dimension);
float
getMinVec4(const std::vector<Vec4f>& vertices, const int dimension);
float
selectValVec4(const Vec4f& vector, const int dimension);

void
saveTexture(const std::vector<Vec3f>& colors, const Vec2u& dimensions, std::string filename);
