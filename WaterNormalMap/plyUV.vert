#version 430 core

layout (location = 0) in vec4 positionModelSpace;
layout (location = 1) in vec3 uv;
layout (location = 2) in vec3 normalModelSpace;

layout (location = 3) uniform mat4 depthBiasMVP;
layout (location = 4) uniform mat4 mvp;
layout (location = 5) uniform vec3 lightPositionWorldSpace;

layout (location = 8) uniform mat4 modelMatrix;
layout (location = 9) uniform mat4 viewMatrix;

out VERTEX {
	vec4 shadowCoord;
	vec3 uv;
	vec3 positionWorldSpace;
	vec3 eyeDirectionCameraSpace;
	vec3 lightDirectionCameraSpace;
	vec3 normalCameraSpace;
	vec3 eye_coord;
} vertex;

void main(void) {
	gl_Position = mvp * vec4(positionModelSpace.xyz, 1.0);

	vertex.positionWorldSpace = (modelMatrix*positionModelSpace).xyz;

	vec3 positionCameraSpace = (viewMatrix*modelMatrix*positionModelSpace).xyz;
	vertex.eye_coord = (viewMatrix*modelMatrix*positionModelSpace).xyz;
	vertex.eyeDirectionCameraSpace = vec3(0,0,0) - positionCameraSpace;

	vec3 lightPositionCameraSpace = (viewMatrix*vec4(lightPositionWorldSpace,1)).xyz;
	vertex.lightDirectionCameraSpace = lightPositionCameraSpace + vertex.eyeDirectionCameraSpace;

	vertex.normalCameraSpace = (viewMatrix*modelMatrix*vec4(normalModelSpace,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.

	vertex.shadowCoord = depthBiasMVP*positionModelSpace;

	vertex.uv = uv;
}