#version 430 core

layout (location = 0) uniform mat4 mvp;
layout (location = 1) uniform vec4 lightPos;

void main(void) {
	gl_Position = lightPos;
}