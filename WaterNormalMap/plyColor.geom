#version 430

layout (triangles) in;

layout (triangle_strip, max_vertices = 3) out;

layout (location = 5) uniform vec3 lightPos;
layout (location = 6) uniform vec3 lightColor;
layout (location = 7) uniform float lightPower;

in vec4[] shadowCoord;

in VERTEX {	
	vec3 color;
	vec3 normal;
} vertex[];

out FRAGMENT {
	vec4 color;
	vec4 shadowCoord;
	float cos;
} fragment;

void main (void) {
	int i;
	
	vec3 lightVec = lightPos.xyz - gl_in[0].gl_Position.xyz;
	float distance = length(lightVec);
	vec3 lightDirection = normalize(lightVec);

	for (i = 0; i < gl_in.length; i++) {
		float cos = clamp(dot(vertex[i].normal.xyz, -lightDirection), 0, 1);
		vec4 diffuseColor = clamp(vec4(vertex[i].color, 1.0), 0.0, 1.0);
		
		vec3 eyeVec = normalize(vec3(0.0, 0.0, 0.0) - gl_in[0].gl_Position.xyz);
		vec3 reflectVec = normalize(reflect(lightVec.xyz, -vertex[i].normal.xyz));
		float cosSpec = clamp(dot(eyeVec, reflectVec), 0, 1);
		vec4 specColor = clamp(vec4(vec3(1.0, 1.0, 1.0)*20*pow(cosSpec, 10)/(distance), 1.0), 0, 1);

		fragment.color = diffuseColor;// + specColor;// + vec4(vec3(0.4, 0.6, 0.4)*faceNormal.y*faceNormal.x, 1.0);
		gl_Position = gl_in[i].gl_Position;
		fragment.shadowCoord = shadowCoord[i];
		fragment.cos = cos;

		EmitVertex();
	}
	EndPrimitive();
}