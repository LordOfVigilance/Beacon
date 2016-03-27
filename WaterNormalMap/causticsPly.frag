#version 430 core

layout (binding = 0) uniform sampler2D shadowMap;
layout (binding = 1) uniform sampler2D waterTexture;

layout (location = 10) uniform float bias;
layout (location = 11) uniform vec2 waterTextureOffset;
layout (location = 12) uniform vec3 lightPos;

uniform vec4 fogColor = vec4(0.7, 0.8, 0.9, 0.0);

in FRAGMENT {
	vec4 color;
	vec4 shadowCoord;
	float cos;
	vec3 positionWorldSpace;
	vec3 eyeDirectionCameraSpace;
	vec3 eye_coord;
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

	//Caustics
	//read depth of vertex
	//read wave texture in direction of light at sea level
	//read blur level based on depth

	
	float waterFactor = 64;
	float waterScale = 32.0;
	//vec2 offset = vec2(x, y) + waterTextureOffset;
	vec2 waterTextureCoord = (fragment.positionWorldSpace.xz + waterTextureOffset*waterScale + vec2(0.5))/waterFactor*2.0;
	
	float waterDepth = clamp(fragment.positionWorldSpace.y*(-1.0)/10.0 - 0.5, 0.0, 1.0);
	float causticColor = clamp(texture( waterTexture, waterTextureCoord, 0).r*waterDepth, 0.0, 1.0)/2;

	if(fragment.positionWorldSpace.y < 0.0) {
		color = vec4(visibility*vec3(fragment.color.r + causticColor*0.5, fragment.color.g + causticColor*0.7, fragment.color.b + causticColor), fragment.color.a);
	}
	else
		color = vec4(visibility*fragment.color.rgb, fragment.color.a);// + causticColor;
	
    //float z = length(fragment.eye_coord)/100;

    //float de = 0.025 * smoothstep(0.0, 6.0, 10.0 - fragment.positionWorldSpace.y);
    //float di = 0.045 * smoothstep(0.0, 40.0, 20.0 - fragment.positionWorldSpace.y);

    //float extinction   = exp(-z * de);
    //float inscattering = exp(-z * di);

	//color = color;//*extinction + fogColor*(1.0 - inscattering);
}