#version 430 core

layout (location = 3) uniform mat4 depthBiasMVP;
layout (location = 4) uniform mat4 mvp;

in vec4 position;

out vec4 shadowCoord;

void main(void) {
	gl_Position = mvp * vec4(position.xyz, 1.0);

	shadowCoord = depthBiasMVP * vec4(position.xyz, 1.0);
}