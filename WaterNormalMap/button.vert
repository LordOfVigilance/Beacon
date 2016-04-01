#version 430 core


layout (location = 0) uniform vec2 size;
layout (location = 1) uniform vec2 offset;


void main(void) {
    vec4 vertices[] = vec4[](vec4( 0.0, 0.0, 0.0, 1.0),
                             vec4( 1.0, 0.0, 0.0, 1.0),
                             vec4( 1.0,-1.0, 0.0, 1.0),
                             vec4( 0.0,-1.0, 0.0, 1.0));
    
    gl_Position = vec4(vertices[gl_VertexID].xy*size - vec2(1.0,-1.0) + vec2(offset.x, -offset.y), vertices[gl_VertexID].zw);
}