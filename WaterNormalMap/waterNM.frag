#version 430 core

out vec4 color;

layout (binding = 0) uniform sampler2D texDisplacement;
uniform vec3 lightPos;
uniform float lightPower;
uniform vec3 lightColor;
uniform vec3 eyePos;

in GS_OUT {
	vec2 textureCoord;
	vec3 normal;
	vec3 position;
} fsIn;

void main(void) {
	vec3 eye_vector = normalize(eyePos - fsIn.position);
	vec3 light_vector = normalize(lightPos - fsIn.position);
	float distance = length(eyePos - lightPos);
	vec3 R = normalize(reflect(light_vector, fsIn.normal));
	float cos = dot(R, eye_vector);

	vec3 specularColor = clamp((lightColor * lightPower * cos)/(distance*distance), 0, 1);

	color = vec4(vec3(texture(texDisplacement, fsIn.textureCoord).r*0.5,
					  texture(texDisplacement, fsIn.textureCoord).r*0.5,
					  texture(texDisplacement, fsIn.textureCoord).r)*fsIn.normal.y + specularColor, 1.0);
}