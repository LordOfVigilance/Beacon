#version 430

layout (triangles) in;

layout (triangle_strip, max_vertices = 3) out;

in VERTEX {
	vec4 color;
} vertex[];

out FRAGMENT {
	vec4 color;
} fragment;

void main (void) {
	int i;

	vec3 edge1 = gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz;
	vec3 edge2 = gl_in[0].gl_Position.xyz - gl_in[2].gl_Position.xyz;

	vec4 face_normal = vec4(normalize(cross(edge1, edge2)), 1.0);

	for (i = 0; i < gl_in.length; i++) {
		fragment.color = vertex[i].color;// * face_normal;
		gl_Position = gl_in[i].gl_Position;

		EmitVertex();
	}
	EndPrimitive();
}