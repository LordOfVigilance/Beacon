#version 430 core


uniform sampler2D myTextureSampler;
layout (location = 3) uniform float alpha;


in vec2 tc;


out vec4 color;


void main(){


    //for depth
    //float colorR = pow(texelFetch( myTextureSampler, ivec2(gl_FragCoord.xy), 0).r, 20);
    //float colorR = texture( myTextureSampler, tc, 0).r;
    //color = vec4(colorR, colorR, colorR, 1.0);


    vec3 textureColor = texture(myTextureSampler, vec2(tc.s, -tc.t)).rgb;


    color = vec4(textureColor, (textureColor.r)*alpha);
}