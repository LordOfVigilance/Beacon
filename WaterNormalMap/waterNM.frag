#version 430 core

out vec4 color;

layout (binding = 0) uniform sampler2D texDisplacement;
layout (binding = 1) uniform sampler2D zBufferTexture;
uniform vec3 lightPos;
uniform float lightPower;
uniform vec3 lightColor;
uniform vec3 eyePos;
uniform vec3 lightPosCorrect;

uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform vec3 reflectionColor;
uniform float var1;
uniform float var2;
uniform float var3;

in GS_OUT {
	vec2 textureCoord;
	vec3 normal;
	vec3 position;
} fsIn;

void main(void) {
	vec3 eye_vector = normalize(vec3(0.0, 0.0, 1.0) - fsIn.position);
	vec3 light_vector = normalize(lightPos - fsIn.position);
	float distance = length(eyePos - lightPos);
	vec3 R = normalize(reflect(light_vector, fsIn.normal));
	float cos = clamp(dot(R, eye_vector), 0, 1);

	vec3 reflectionColorComp = clamp(reflectionColor * 20 * pow(cos, 1.0)/(distance*distance), 0, 1);

	
	vec3 eyeVecCorr = normalize(vec3(0.0, 0.0, 0.0) - fsIn.position);
	vec3 lightVecCorr = normalize(lightPosCorrect - fsIn.position);
	float distanceCorr = length(lightPosCorrect);
	vec3 reflectCorr = normalize(reflect(-lightVecCorr, fsIn.normal));
	float cosCorr = clamp(dot(eyeVecCorr, reflectCorr), 0, 1);

	float powCosCorr = pow(cosCorr, 60);
	vec3 specularColorComp = clamp(specularColor * 200 * powCosCorr/(distanceCorr), 0, 1);
	vec3 diffuseColorComp = clamp(diffuseColor, 0, 1);

	float texColor = texture(texDisplacement, fsIn.textureCoord).r;
	vec3 texColorComp = vec3(texColor, texColor, texColor);

	float alphaComp = (1 - clamp(dot(fsIn.normal, normalize(fsIn.position)), 0, 1))/2;		//2
	float alpha2 = dot(fsIn.normal, normalize(fsIn.position))*var2;							//0.6

	alphaComp = max(alphaComp, alpha2);
	
	float zFrag = gl_FragCoord.z;
	float fragDist = length(fsIn.position);
	float zTexel = texelFetch(zBufferTexture, ivec2(gl_FragCoord.xy), 0).r;
	
	//Gives Sharp Lines around edges
	float cameraTransparency = 4;
	float cameraEdgeFactor = 0.015;
	float edgeFactor = 2.0;
	float zDifference = 1 - pow(zFrag/zTexel, cameraTransparency + fragDist*fragDist*cameraEdgeFactor);
	zDifference = clamp(round(sqrt(zDifference)*edgeFactor), 0, 1)/2 + 1.0;

	//Can see through shallow water
	//float zDifference = 1 - abs((zFrag/zTexel/(zFrag+zTexel) - var3))*var1;
	//zDifference = clamp(round(sqrt(zDifference)*var2), 0, 1)/2 + 0.5;

	color = vec4(specularColorComp + reflectionColorComp + diffuseColorComp, zDifference*alphaComp);
}