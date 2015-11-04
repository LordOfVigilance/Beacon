#version 430

layout (triangles) in;

layout (triangle_strip, max_vertices = 3) out;

layout (location = 5) uniform vec3 lightPos;
layout (location = 6) uniform vec3 lightColor;
layout (location = 7) uniform float lightPower;

out FRAGMENT {
	vec4 color;
} fragment;

void main (void) {
	int i;

	vec3 edge1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 edge2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

	vec3 faceNormal = normalize(cross(edge2, edge1));
	
	vec3 lightVec = lightPos.xyz - gl_in[0].gl_Position.xyz;
	float distance = length(lightVec);
	vec3 lightDirection = normalize(lightVec);
	float cos = clamp(dot(lightDirection, faceNormal), 0, 1);
	vec4 diffuseColor = clamp(vec4(vec3(0.3, 0.5, 0.6)*lightColor*lightPower*cos, 1.0), 0.0, 0.5);

	vec3 eyeVec = normalize(vec3(0.0, 0.0, 0.0) - gl_in[0].gl_Position.xyz);
	vec3 reflectVec = normalize(reflect(-lightDirection, faceNormal));
	float cosSpec = clamp(dot(eyeVec, reflectVec), 0, 1);
	vec4 specColor = clamp(vec4(vec3(1.0, 1.0, 1.0)*100*pow(cosSpec, 10)/(distance), 1.0), 0, 1);

	for (i = 0; i < gl_in.length; i++) {
		fragment.color = diffuseColor + specColor;
		gl_Position = gl_in[i].gl_Position;

		EmitVertex();
	}
	EndPrimitive();
}