#version 430

layout (triangles) in;

layout (triangle_strip, max_vertices = 3) out;

in VERTEX {
	vec4 shadowCoord;
	vec3 uv;
	vec3 positionWorldSpace;
	vec3 eyeDirectionCameraSpace;
	vec3 lightDirectionCameraSpace;
	vec3 normalCameraSpace;
	vec3 eye_coord;
} vertex[];

out FRAGMENT {
	vec4 shadowCoord;
	vec3 uv;
	vec3 positionWorldSpace;
	vec3 eyeDirectionCameraSpace;
	vec3 lightDirectionCameraSpace;
	vec3 normalCameraSpace;
	vec3 eye_coord;
} fragment;

void main (void) {
	int i;
	for (i = 0; i < gl_in.length; i++) {
		gl_Position = gl_in[i].gl_Position;
		
		fragment.shadowCoord = vertex[i].shadowCoord;
		fragment.uv = vertex[i].uv;
		fragment.positionWorldSpace = vertex[i].positionWorldSpace;
		fragment.eyeDirectionCameraSpace = vertex[i].eyeDirectionCameraSpace;
		fragment.lightDirectionCameraSpace = vertex[i].lightDirectionCameraSpace;
		fragment.normalCameraSpace = vertex[i].normalCameraSpace;
		fragment.eye_coord = vertex[i].eye_coord;

		EmitVertex();
	}
	EndPrimitive();
}