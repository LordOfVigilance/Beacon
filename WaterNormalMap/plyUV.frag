#version 430 core

layout (binding = 0) uniform sampler2D shadowMap;
layout (binding = 1) uniform sampler2D textureMap;


layout (location = 5) uniform vec3 lightPositionWorldSpace;
layout (location = 6) uniform vec3 lightColor;
layout (location = 7) uniform float lightPower;
layout (location = 10) uniform float bias;

uniform vec4 fogColor = vec4(0.7, 0.8, 0.9, 0.0);

in FRAGMENT {
	vec4 shadowCoord;
	vec3 uv;
	vec3 positionWorldSpace;
	vec3 eyeDirectionCameraSpace;
	vec3 lightDirectionCameraSpace;
	vec3 normalCameraSpace;
	vec3 eye_coord;
} fragment;

out vec4 color;

float computeShadow() {
	float visibility = 1.0;
	//float bias = 0.005*tan(acos(fragment.cos));
	//bias = clamp(bias, 0.0,0.01);

	if (fragment.shadowCoord.x < 0.0 || fragment.shadowCoord.x > 1.0 || fragment.shadowCoord.y < 0.0 || fragment.shadowCoord.y > 1.0) {
		visibility = 1.0;
	} else if(texture( shadowMap, fragment.shadowCoord.xy ).z  <  fragment.shadowCoord.z - bias){
		visibility = 0.5;
	}

	return visibility;
}

void main () {
	// Material properties
	vec3 materialDiffuseColor = vec3(0.5, 0.6, 1.0);
	vec3 materialAmbientColor = vec3(0.2,0.2,0.2)*materialDiffuseColor;
	vec3 materialSpecularColor = vec3(1.0, 1.0, 1.0);
	
	// Distance to the light
	float distanceToLight = length(lightPositionWorldSpace - fragment.positionWorldSpace);

	vec3 normal = normalize(fragment.normalCameraSpace);
	vec3 lightDirection = normalize(fragment.lightDirectionCameraSpace);
	float cosTheta = clamp(dot(normal, lightDirection), 0, 1);

	vec3 eyeDirection = normalize(fragment.eyeDirectionCameraSpace);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float cosAlpha = clamp( dot(eyeDirection, reflectionDirection), 0,1 );
	
	vec3 fragmentColor = materialSpecularColor * lightColor * pow(cosAlpha, 3);

	float visibility = computeShadow();
	vec3 textureColor = texture(textureMap, fragment.uv.xy, 0).rgb;
	vec3 textureComponent = textureColor * 1.4;

	color = vec4(visibility*(fragmentColor + textureComponent), (1 - textureColor.r)*0.5 + 0.5);
}