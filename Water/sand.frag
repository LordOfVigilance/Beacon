#version 430 core

in FRAGMENT {
	vec4 color;
} fragment;

out vec4 frag_colour;

void main () {
	frag_colour = fragment.color;
}