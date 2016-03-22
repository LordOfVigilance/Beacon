#version 430 core

layout (location = 3) uniform mat4 depthBiasMVP;
layout (location = 4) uniform mat4 mvp;

layout (location = 8) uniform float sphereFalloff;
layout (location = 9) uniform float sphereFactor;
layout (location = 11) uniform vec3 spherePosition;

in vec4 position;

out vec4 shadowCoord;

void main() {
	vec4 vertPosition = mvp*vec4(position.xyz, 1.0);

	vec3 sphereVector = position.xyz - spherePosition;
	float sphereDistance = dot(sphereVector, sphereVector);
	vec3 sphereDirection = sphereVector/sphereDistance;

	vec3 sphereVertPosition = position.xyz + vec3(0.0, 0.0, -5.0) + sphereDirection*sphereFactor;
	gl_Position = mvp*vec4(sphereVertPosition, 1.0);

	shadowCoord = depthBiasMVP*vec4(position.xyz, 1.0);
}