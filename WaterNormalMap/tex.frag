#version 430 core

uniform sampler2D myTextureSampler;

in vec2 tc;

out vec4 color;

void main(){

	//for depth
	float colorR = pow(texelFetch( myTextureSampler, ivec2(gl_FragCoord.xy), 0).r, 20);
    color = vec4(colorR, colorR, colorR, 1.0);

	//color = texture(myTextureSampler, tc, 0);
}