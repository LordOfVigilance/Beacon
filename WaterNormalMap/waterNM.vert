#version 430 core

uniform vec2 texOffset;

out VS_OUT {
	vec2 textureCoord;
	vec2 foamTextureCoord;
	vec2 foamTextureCoord2;
} vsOut;

void main(void) {
	int waterFactor = 128;
	int waterFactorLog = 7;
	float waterScale = 64.0;

	const vec4 vertices[] = vec4[](vec4(-0.5, 0.0, -0.5, 1.0),
								   vec4( 0.5, 0.0, -0.5, 1.0),
								   vec4(-0.5, 0.0,  0.5, 1.0),
								   vec4( 0.5, 0.0,  0.5, 1.0));

	int x = gl_InstanceID & (waterFactor - 1);
	int y = gl_InstanceID >> waterFactorLog;
	vec2 offset = vec2(x, y) + texOffset;

	vsOut.textureCoord = waterScale*(vertices[gl_VertexID].xz + offset + vec2(0.5)) / (waterFactor*1.0);
	vsOut.foamTextureCoord = waterScale*(vertices[gl_VertexID].xz - offset*0.2 + vec2(0.5)) / (waterFactor*1.0);
	vsOut.foamTextureCoord2 = waterScale*(vertices[gl_VertexID].xz + offset/2 + vec2(0.5)) / (waterFactor*1.0);
	vec4 instancePosition = vec4(float(x - waterFactor/2), 0.0, float(y - waterFactor/2), 0.0);
	gl_Position = vec4(waterScale*(vertices[gl_VertexID] + instancePosition).xyz, 1.0);
}