#version 430 core

layout (location = 0) out float fragmentDepth;

void main(void) {
	float farPlane = 1;
	fragmentDepth = gl_FragCoord.z/gl_FragCoord.w*farPlane;
}