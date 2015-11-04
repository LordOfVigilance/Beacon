#version 430 core

layout (location = 2) uniform vec4 colorIn;

out vec4 color;

void main(void) {
	color = colorIn;
}