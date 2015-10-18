#version 430 core

in vec4 position;

layout (location = 4) uniform mat4 mvp;

void main(void) {
	gl_Position = mvp * position;
}