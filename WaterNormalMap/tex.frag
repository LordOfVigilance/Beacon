#version 430 core

uniform sampler2D myTextureSampler;

out vec4 color;

void main(){

	//for depth
	//float colorR = pow(texelFetch( myTextureSampler, ivec2(gl_FragCoord.xy), 0).r, 20);
    //color = vec4(colorR, colorR, colorR, 1.0);

	color = texelFetch(myTextureSampler, ivec2(gl_FragCoord.xy), 0);
}