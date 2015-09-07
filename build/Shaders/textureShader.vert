#version 330 core

in vec4 position;
in vec2 texCoord;

layout(std140) uniform GlobalMatrices
{
  mat4 cameraToClipMatrix;	// Perspective Matrix
  mat4 worldToCameraMatrix;	// CameraMatrix
  mat4 localToWorldMatrix;	// LocalMatrix

  unsigned int debugCounter;	// Counter
};

out vec2 Texcoord;

void main() {
  gl_Position =   cameraToClipMatrix * worldToCameraMatrix * localToWorldMatrix * position;
  Texcoord = texCoord;
}
