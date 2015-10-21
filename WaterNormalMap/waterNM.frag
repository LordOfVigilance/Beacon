#version 430 core

out vec4 color;

layout (binding = 0) uniform sampler2D texDisplacement;
uniform vec3 lightPos;
uniform float lightPower;
uniform vec3 lightColor;
uniform vec3 eyePos;
uniform vec3 lightPosCorrect;

in GS_OUT {
	vec2 textureCoord;
	vec3 normal;
	vec3 position;
} fsIn;

void main(void) {
	vec3 eye_vector = normalize(vec3(0.0, 0.0, 10.0) - fsIn.position);
	vec3 light_vector = normalize(lightPos - fsIn.position);
	float distance = length(eyePos - lightPos);
	vec3 R = normalize(reflect(light_vector, fsIn.normal));
	float cos = clamp(dot(R, eye_vector), 0, 1);

	vec3 reflectionColor = clamp((vec3(0.8, 0.0, 0.0) * 20 * pow(cos,5))/(distance*distance), 0, 1);

	
	vec3 eyeVecCorr = normalize(vec3(0.0, 0.0, 0.0) - fsIn.position);
	vec3 lightVecCorr = normalize(lightPosCorrect - fsIn.position);
	float distanceCorr = length(lightPosCorrect);
	vec3 reflectCorr = normalize(reflect(-lightVecCorr, fsIn.normal));
	float cosCorr = clamp(dot(eyeVecCorr, reflectCorr), 0, 1);

	float powCosCorr = pow(cosCorr, 20);
	vec3 specularColor = clamp((vec3(1.0, 0.7, 0.3) * 500 * powCosCorr)/(distanceCorr), 0, 1);	

	float texColor = texture(texDisplacement, fsIn.textureCoord).r;

	float alpha = (1 - clamp(dot(fsIn.normal, normalize(fsIn.position)), 0, 1))/3;
	float alpha2 = clamp(dot(fsIn.normal, normalize(fsIn.position))+0.6, 0, 1);

	alpha = max(alpha, alpha2);
	alpha = max(alpha, powCosCorr/2);

	color = vec4(vec3(0.2*(1-texColor) + 0.1*texColor,
					  0.01*(1-texColor) + 0.1*texColor,
					  0.2*(1-texColor) + 0.3*texColor)*fsIn.normal.y
					  + specularColor + reflectionColor, alpha);
}