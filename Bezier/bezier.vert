#version 430 core

in vec4 position;

layout (location = 0) uniform mat4 mv_matrix;

void main (void)
{
	gl_Position = mv_matrix * position;
}