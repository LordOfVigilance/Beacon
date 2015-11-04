#version 430 core

out vec2 UV;

void main(void) {
	const vec4 vertices[] = vec4[]( vec4(-0.5,  0.5, 0.0, 1.0),
									vec4( 0.5,  0.5, 0.0, 1.0),
									vec4(-0.5, -0.5, 0.0, 1.0),
									vec4( 0.5, -0.5, 0.0, 1.0));

	const vec2 uvs[] = vec2[](vec2(0.0, 1.0),
							  vec2(1.0, 1.0),
							  vec2(0.0, 0.0),
							  vec2(1.0, 0.0));

	gl_Position = vertices[gl_VertexID];

	UV = uvs[gl_VertexID];
}