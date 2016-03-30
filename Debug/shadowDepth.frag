#version 430 core

//out
layout(location = 1) out float fragmentdepth;

void main() {
	fragmentdepth = gl_FragCoord.z;
}