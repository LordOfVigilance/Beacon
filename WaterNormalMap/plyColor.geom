#version 430

layout (triangles) in;

layout (triangle_strip, max_vertices = 3) out;

layout (location = 5) uniform vec3 lightPositionWorldSpace;
layout (location = 6) uniform vec3 lightColor;
layout (location = 7) uniform float lightPower;

in VERTEX {
	vec4 shadowCoord;
	vec3 color;
	vec3 positionWorldSpace;
	vec3 eyeDirectionCameraSpace;
	vec3 lightDirectionCameraSpace;
	vec3 normalCameraSpace;
} vertex[];

out FRAGMENT {
	vec4 color;
	vec4 shadowCoord;
	float cos;
} fragment;

void main (void) {
	
	// Material properties
	vec3 materialDiffuseColor = vertex[0].color;
	vec3 materialAmbientColor = vec3(0.1,0.1,0.1)*materialDiffuseColor;
	vec3 materialSpecularColor = vec3(0.3,0.3,0.3);
	
	// Distance to the light
	float distanceToLight = length(lightPositionWorldSpace - vertex[0].positionWorldSpace);

	vec3 normal = normalize(vertex[0].normalCameraSpace);
	vec3 lightDirection = normalize(vertex[0].lightDirectionCameraSpace);
	float cosTheta = clamp(dot(normal, lightDirection), 0, 1);

	vec3 eyeDirection = normalize(vertex[0].eyeDirectionCameraSpace);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float cosAlpha = clamp( dot( eyeDirection, reflectionDirection), 0,1 );
	
	vec3 color = materialAmbientColor + materialDiffuseColor*lightColor*lightPower*cosTheta + materialSpecularColor*lightColor*lightPower*pow(cosAlpha,5);
	
	int i;
	for (i = 0; i < gl_in.length; i++) {
		gl_Position = gl_in[i].gl_Position;

		fragment.color = vec4(color, 1.0);
		fragment.shadowCoord = vertex[i].shadowCoord;
		fragment.cos = 1.0;

		EmitVertex();
	}
	EndPrimitive();
}