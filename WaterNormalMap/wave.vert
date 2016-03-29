#version 430 core

layout (location = 0) in vec4 positionModelSpace;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normalModelSpace;

layout (location = 3) uniform mat4 depthBiasMVP;
layout (location = 4) uniform mat4 mvp;
layout (location = 5) uniform vec3 lightPositionWorldSpace;

layout (location = 8) uniform mat4 modelMatrix;
layout (location = 9) uniform mat4 viewMatrix;

layout (location = 13) uniform float sphereFalloff;
layout (location = 14) uniform float sphereFactor;
layout (location = 15) uniform vec3 spherePosition;

out VERTEX {
	vec4 shadowCoord;
	vec3 color;
	vec3 positionWorldSpace;
	vec3 eyeDirectionCameraSpace;
	vec3 lightDirectionCameraSpace;
	vec3 normalCameraSpace;
	vec3 eye_coord;
	vec3 positionModelSpace;
} vertex;

void main(void) {
	vec4 vertPosition = mvp*vec4(positionModelSpace.xyz, 1.0);

	vec3 sphereVector = positionModelSpace.xyz - spherePosition;
	float sphereDistance = dot(sphereVector, sphereVector);
	vec3 sphereDirection = sphereVector/sphereDistance;

	vec3 sphereVertPosition = positionModelSpace.xyz + sphereDirection*sphereFactor;
	gl_Position = mvp * vec4(sphereVertPosition, 1.0);

	vertex.positionModelSpace = positionModelSpace.xyz;
	vertex.positionWorldSpace = (modelMatrix*positionModelSpace).xyz;

	vec3 positionCameraSpace = (viewMatrix*modelMatrix*positionModelSpace).xyz;
	vertex.eye_coord = (viewMatrix*modelMatrix*positionModelSpace).xyz;
	vertex.eyeDirectionCameraSpace = vec3(0,0,0) - positionCameraSpace;

	vec3 lightPositionCameraSpace = (viewMatrix*vec4(lightPositionWorldSpace,1)).xyz;
	vertex.lightDirectionCameraSpace = lightPositionCameraSpace + vertex.eyeDirectionCameraSpace;

	vertex.normalCameraSpace = (viewMatrix*modelMatrix*vec4(normalModelSpace,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.

	vertex.shadowCoord = depthBiasMVP*positionModelSpace;

	vertex.color = color;
}