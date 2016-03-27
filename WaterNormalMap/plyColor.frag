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

	color = vec4(visibility*fragment.color.rgb, fragment.color.a);
	
    //float z = length(fragment.eye_coord)/100;

    //float de = 0.025 * smoothstep(0.0, 6.0, 10.0 - fragment.positionWorldSpace.y);
    //float di = 0.045 * smoothstep(0.0, 40.0, 20.0 - fragment.positionWorldSpace.y);

    //float extinction   = exp(-z * de);
    //float inscattering = exp(-z * di);

	//color = color;//*extinction + fogColor*(1.0 - inscattering);
}