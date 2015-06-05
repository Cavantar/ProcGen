#pragma once

#include "Includes.h"


typedef vector<list<int>> AdjacencyList;
//void log() {
//	static bool multiThreaded = true;
//	
//	if(!multiThreaded){
//		cout << 
//	}
//}


bool glCheckErrors();
#define  GL_CHECK_ERRORS if(glCheckErrors()) assert(0);

void showVec3(const glm::vec3& vector);
void showVec4(const glm::vec4& vector, bool tag = false);
void showMat4(const glm::mat4& matrix);

enum RENDER_TYPE{
	RT_POINTS,
	RT_LINES,
	RT_TRIANGLES
};

float dotProduct(const glm::vec3& v1, const glm::vec3& v2);
float dotProduct(const glm::vec2& v, const float x, const float y);
float interpFloat(const float x0, const float x1, const float t);


// TODO FIX ADJACENCY LIST FOR CYLINDER
AdjacencyList createGridAdjacencyList(const vector<glm::vec4>& vertices, const glm::uvec2& dimensions, bool loop = false);
vector<glm::uvec2> createGridLineIndex(const int width, const int height);
vector<glm::uvec3> createGridTriangleIndex(const int width, const int height);

vector<glm::vec3> getNormals(const vector<glm::vec4>& vertices, vector<glm::uvec3>& faceIndex, AdjacencyList& adjacencyList = AdjacencyList());
vector<glm::vec3> calculateFaceNormals(const vector<glm::vec4>& vertices, vector<glm::uvec3>& faceIndex);

list<int> getTriangleIndexes(int srcIndex, vector<glm::uvec3>& faceIndex);
void reverseTriangleIndexes(vector<glm::uvec3>& triangles);
void addResersedTriangleIndexes(vector<glm::uvec3>& triangles);
void addIndexedQuad(vector<glm::uvec3>& triangles, glm::uvec4 indexes);

void translateVec4(vector<glm::vec4>& vertices, glm::vec4 delta);
glm::vec2 getVec4Bounds(const vector<glm::vec4>& vertices, const int dimension);
float getMaxVec4(const vector<glm::vec4>& vertices, const int dimension);
float getMinVec4(const vector<glm::vec4>& vertices, const int dimension);
float selectValVec4(const glm::vec4& vector, const int dimension);