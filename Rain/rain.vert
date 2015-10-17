#version 430 core

in vec4 position;
in vec4 instance_color;
in vec4 instance_offset;

out VERTEX {
	vec4 color;
} vertex;

layout (location = 4) uniform mat4 mvp;
layout (location = 5) uniform vec4 light_pos;
layout (location = 6) uniform vec4 light_color;
layout (location = 7) uniform float light_power;
layout (location = 8) uniform vec4 offset;

void main(void) {
	vec4 vertex_pos = position + instance_offset * 4 + offset;

	gl_Position = mvp * vertex_pos;

	//diffuse color
	float distance = dot(vertex_pos - light_pos, vertex_pos - light_pos);
	vec4 diffuse_color = light_color * light_power * instance_color / (distance * distance);

	vertex.color = clamp (diffuse_color, instance_color * 0.01, vec4(1.0));
}