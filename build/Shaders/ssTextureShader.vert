#version 330 core

in vec4 position;
in vec2 texCoord;

out vec2 Texcoord;

void main() {
  gl_Position = position;
  Texcoord = texCoord;
}
