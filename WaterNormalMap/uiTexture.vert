#version 430 core

layout (location = 0) uniform ivec2 screenSize;
layout (location = 1) uniform vec2 textureSize;
layout (location = 2) uniform ivec2 texturePosition;

out vec2 tc;

void main(void) {
	const vec2 vertices[] = vec2[]( vec2(-1.0,  1.0),
									vec2( 1.0,  1.0),
									vec2( 1.0, -1.0),
									vec2(-1.0, -1.0));

	tc = vertices[gl_VertexID].xy/2 + 0.5;

	float xPosition = vertices[gl_VertexID].x/screenSize.x*textureSize.x + texturePosition.x*2.0/screenSize.x - 1.0;
	float yPosition = vertices[gl_VertexID].y/screenSize.y*textureSize.y + texturePosition.y*2.0/screenSize.y - 1.0;
	vec2 position = vec2(xPosition, yPosition);
	gl_Position = vec4(position.xy, 1.0, 1.0);
}