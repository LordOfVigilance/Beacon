#version 430 core

in vec4 position;

layout (location = 1) uniform mat4 model;
layout (location = 4) uniform mat4 mvp;

layout (location = 2) uniform vec4 clippingPlane = vec4(0.0, 2.0, 0.0, 0.0);

void main(void) {
	gl_ClipDistance[0] = dot(model*position, clippingPlane);

	gl_Position = mvp*position;
}