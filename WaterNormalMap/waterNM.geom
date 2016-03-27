#version 430 core

layout (triangles) in;

layout (triangle_strip, max_vertices = 3) out;

in TES_OUT {
	vec2 textureCoord;
	vec2 foamTextureCoord;
	vec2 foamTextureCoord2;
	vec4 worldPosition;
} gsIn[];

out GS_OUT {
	vec2 textureCoord;
	vec2 foamTextureCoord;
	vec2 foamTextureCoord2;
	vec3 normal;
	vec3 position;
	vec4 worldPosition;
} gsOut;

void main(void) {
	vec3 edge1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 edge2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

	vec3 face_normal = normalize(cross(edge2, edge1));

	int i;

	for (i = 0; i < gl_in.length; i++) {
		gsOut.textureCoord = gsIn[i].textureCoord;
		gsOut.foamTextureCoord = gsIn[i].foamTextureCoord;
		gsOut.foamTextureCoord2 = gsIn[i].foamTextureCoord2;
		gsOut.normal = face_normal;
		gsOut.position = gl_in[i].gl_Position.xyz;
		gsOut.worldPosition = gsIn[i].worldPosition;
		gl_Position = gl_in[i].gl_Position;

		EmitVertex();
	}
	EndPrimitive();
}