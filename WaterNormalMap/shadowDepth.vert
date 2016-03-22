#version 430 core

//in
layout (location = 0) in vec3 vertexPosition_modelspace;

layout (location = 4) uniform mat4 depthMVP;

void main() {
	gl_Position = depthMVP * vec4(vertexPosition_modelspace, 1);
	
}