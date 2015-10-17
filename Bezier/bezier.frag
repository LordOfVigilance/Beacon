#version 430 core

out vec4 color;

in TES_OUT
{
	vec3 N;
	vec4 color;
} fs_in;

void main (void)
{
	vec3 N = normalize(fs_in.N);

	vec4 c = vec4 (0.2, 0.0, 1.0, 0.0) * N.x + vec4(0.0, 0.0, 0.3, 1.0) + vec4(0.2, -0.5, -1.0, 1.0) * N.x;

	color = fs_in.color;//clamp (c, vec4(0.0), vec4 (1.0));
}