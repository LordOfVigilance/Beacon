#version 430 core

in vec4 position;

out VERTEX {
	vec4 color;
} vertex;

layout (location = 4) uniform mat4 mvp;
layout (location = 5) uniform vec4 light_pos;
layout (location = 6) uniform vec4 light_color;
layout (location = 7) uniform float light_power;
layout (location = 8) uniform vec4 color;

void main(void) {
	vec4 vertex_pos = position;

	gl_Position = mvp * vertex_pos;

	//diffuse color
	float distance = dot(vertex_pos - light_pos, vertex_pos - light_pos);
	vec4 diffuse_color = light_color * light_power * color / (distance);

	vertex.color = clamp (diffuse_color, color * 0.01, vec4(1.0));
}