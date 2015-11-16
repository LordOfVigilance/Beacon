#version 430 core

uniform vec2 texOffset;

out VS_OUT {
	vec2 textureCoord;
} vsOut;

void main(void) {
	const vec4 vertices[] = vec4[](vec4(-0.5, 0.0, -0.5, 1.0),
								   vec4( 0.5, 0.0, -0.5, 1.0),
								   vec4(-0.5, 0.0,  0.5, 1.0),
								   vec4( 0.5, 0.0,  0.5, 1.0));

	int x = gl_InstanceID & 255;
	int y = gl_InstanceID >> 8;
	vec2 offset = vec2(x, y) + texOffset;

	vsOut.textureCoord = (vertices[gl_VertexID].xz + offset + vec2(0.5)) / 32.0;
	gl_Position = vertices[gl_VertexID] + vec4(float(x - 128), 0.0, float(y - 128), 0.0);
}