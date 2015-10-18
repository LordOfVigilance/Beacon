#version 430

layout (triangles) in;

layout (triangle_strip, max_vertices = 3) out;

layout (location = 5) uniform vec4 lightPos;
layout (location = 6) uniform vec3 lightColor;
layout (location = 7) uniform float lightPower;

out FRAGMENT {
	vec4 color;
} fragment;

void main (void) {
	int i;

	vec3 edge1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 edge2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

	vec4 faceNormal = vec4(normalize(cross(edge1, edge2)), 1.0);
	
	vec3 lightVec = lightPos.xyz - gl_in[0].gl_Position.xyz;
	float distance = length(lightVec);
	vec3 lightDirection = normalize(lightVec);
	float cos = clamp(dot(faceNormal.xyz, lightDirection), 0, 1);
	vec4 diffuseColor = vec4(vec3(0.3, 0.5, 0.6)*lightColor*lightPower*cos/(distance), 1.0);

	vec3 eyeVec = normalize(vec3(0.0, 0.0, 0.0) - gl_in[0].gl_Position.xyz);
	vec3 reflectVec = normalize(reflect(-lightVec.xyz, faceNormal.xyz));
	float cosSpec = clamp(dot(eyeVec, reflectVec), 0, 1);
	vec4 specColor = clamp(vec4(vec3(1.0, 1.0, 1.0)*1000*pow(cosSpec, 50)/(distance*distance), 1.0), 0, 1);

	for (i = 0; i < gl_in.length; i++) {
		fragment.color = diffuseColor + specColor + vec4(0.4, 0.6, 0.4, 1.0)*faceNormal.y*faceNormal.x;
		gl_Position = gl_in[i].gl_Position;

		EmitVertex();
	}
	EndPrimitive();
}