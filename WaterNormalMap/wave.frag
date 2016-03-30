#version 430 core

layout (binding = 0) uniform sampler2D shadowMap;
layout (binding = 1) uniform sampler2D waveTexture;

layout (location = 10) uniform float bias;
layout (location = 11) uniform vec2 waterTextureOffset;
layout (location = 12) uniform vec3 lightPos;
layout (location = 16) uniform vec3 skyColor;
layout (location = 17) uniform vec2 textureOffset;

uniform vec4 fogColor = vec4(0.7, 0.8, 0.9, 0.0);
uniform float var4 = 340.0f;
uniform float var5 = 580.0f;
uniform float var6 = 260.0f;

uniform int screenWidth = 1280;
uniform int screenHeight = 720;

in FRAGMENT {
	vec4 color;
	vec4 shadowCoord;
	float cos;
	vec3 positionWorldSpace;
	vec3 eyeDirectionCameraSpace;
	vec3 lightDirectionCameraSpace;
	vec3 normalCameraSpace;
	vec3 eye_coord;
	vec3 positionModelSpace;
} fragment;

out vec4 color;

void main () {
	float visibility = 1.0;
	//float bias = 0.005*tan(acos(fragment.cos));
	//bias = clamp(bias, 0.0,0.01);

	if (fragment.shadowCoord.x < 0.0 || fragment.shadowCoord.x > 1.0 || fragment.shadowCoord.y < 0.0 || fragment.shadowCoord.y > 1.0) {
		visibility = 1.0;
	} else if(texture( shadowMap, fragment.shadowCoord.xy ).z  <  fragment.shadowCoord.z - bias){
		visibility = 0.5;
	}
	
	float waterFactor = 64;
	float waterScale = 32.0;
	//vec2 offset = vec2(x, y) + waterTextureOffset;
	vec2 waterTextureCoord = (fragment.positionWorldSpace.xz + waterTextureOffset*waterScale + vec2(0.5))/waterFactor*2.0;
	
	float waterDepth = clamp(fragment.positionWorldSpace.y*(-1.0)/10.0 - 0.5, 0.0, 1.0);
	float waveTextureColor = clamp(texture( waveTexture, fragment.positionModelSpace.xz + textureOffset, 0).r, 0.0, 1.0);

	float alphaFeather = clamp(fragment.positionWorldSpace.y, 0.0, 1.0);

	color = vec4(fragment.color.rgb + waveTextureColor, alphaFeather);
}