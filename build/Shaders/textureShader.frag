#version 330 core

in vec2 Texcoord;
out vec4 fragColor;
uniform sampler2D tex;

void main() {
  fragColor = texture(tex, Texcoord);
  //fragColor = vec4(1.0, 0, 0, 1.0);
}
