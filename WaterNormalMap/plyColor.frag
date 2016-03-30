#version 430 core

layout (binding = 0) uniform sampler2D shadowMap;

layout (location = 10) uniform float bias;

uniform vec4 fogColor = vec4(0.7, 0.8, 0.9, 0.0);

in FRAGMENT {
	vec4 color;
	vec4 shadowCoord;
	float cos;
	vec3 positionWorldSpace;
	vec3 eyeDirectionCameraSpace;
	vec3 lightDirectionCameraSpace;
	vec3 normalCameraSpace;
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

	color = vec4(fragment.color.rgb, fragment.color.a);
}