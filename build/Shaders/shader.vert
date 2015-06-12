#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

smooth out vec4 smoothColor;
uniform mat4 MVP;

void main() {
	smoothColor = vec4(color, 1.0);
	gl_Position = MVP*vec4(position, 1.0);
}
