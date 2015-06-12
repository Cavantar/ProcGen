#pragma once 

#include "Includes.h"
#include "GLSLShader.h"

class TexturedQuad{
public:
	void prepareData(const vector<glm::vec3>& textureData, const int textureWidth, const float objectWidth);
	void copyToGfx(GLSLShader& shader);

	void render(const RENDER_TYPE renderType, const GLuint texBindingUnit) const;
private:
	vector<glm::vec3> textureData;
	int textureWidth;
	
	vector<glm::vec4> vertices;
	vector<glm::vec2> texCoords;
	vector<glm::uvec3> triangleIndexVec;

	vector<float> rawData;

	GLuint textureBufferObject;
	GLuint texDataBufferObject, texTriangleIndexBuffer, texVao;

};