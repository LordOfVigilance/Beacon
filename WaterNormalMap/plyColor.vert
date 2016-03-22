#version 430 core

layout (location = 3) uniform mat4 depthBiasMVP;
layout (location = 4) uniform mat4 mvp;

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;

out vec4 shadowCoord;

out VERTEX {
	vec3 color;
	vec3 normal;
} vertex;

void main(void) {
	gl_Position = mvp * vec4(position.xyz, 1.0);

	shadowCoord = depthBiasMVP * vec4(position.xyz, 1.0);

	vertex.color = color;
	vertex.normal = (mvp*vec4(normal, 1.0)).xyz;
}