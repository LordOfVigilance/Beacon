#version 430 core

out vec4 color;

layout (binding = 0) uniform sampler2D texDisplacement;
layout (binding = 1) uniform sampler2D texReflectionColor;
layout (binding = 2) uniform sampler2D zBufferTexture;
layout (binding = 3) uniform sampler2D texReflectionDepth;
layout (binding = 4) uniform sampler2D normalMap;
layout (binding = 5) uniform sampler2D worldDepthMap;
layout (binding = 6) uniform sampler2D foamTexture;
layout (binding = 7) uniform sampler2D worldDepthMapFoam;

uniform float dmap_depth;
uniform vec3 lightPos;
uniform float lightPower;
uniform vec3 lightColor;
uniform vec3 eyePos;
uniform vec3 lightPosCorrect;

uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform vec3 reflectionColor;
uniform float var1;
uniform float var2;
uniform float var3;
uniform float var4;
uniform float var5;
uniform float var6;

uniform int screenWidth = 1280;
uniform int screenHeight = 720;
uniform vec3 skyColor = vec3(0.78, 0.91, 1.0);

in GS_OUT {
	vec2 textureCoord;
	vec2 foamTextureCoord;
	vec2 foamTextureCoord2;
	vec3 normal;
	vec3 position;
	vec4 worldPosition;
} fsIn;

void main(void) {
	vec3 eye_vector = normalize(vec3(0.0, 0.0, 1.0) - fsIn.position);
	vec3 light_vector = normalize(lightPos - fsIn.position);
	float distance = length(eyePos - lightPos);
	vec3 R = normalize(reflect(light_vector, fsIn.normal));
	float cos = clamp(dot(R, eye_vector), 0, 1);
	vec3 reflectionColorComp = clamp(reflectionColor * 20 * pow(cos, 1.0)/(distance*distance), 0, 1);
	
	vec3 normal = normalize(texture(normalMap, fsIn.textureCoord).rgb*2.0 - vec3(1.0));

	vec3 eyeVecCorr = normalize(vec3(0.0, 0.0, 0.0) - fsIn.position);
	vec3 lightVecCorr = normalize(lightPosCorrect - vec3(0.0, 0.0, 0.0));
	float distanceCorr = length(lightPosCorrect);
	vec3 reflectCorr = normalize(reflect(-lightVecCorr, fsIn.normal));
	float cosCorr = max(dot(reflectCorr, eyeVecCorr), 0.0);
	float specularPower = var1;
	float specularTightness = var2;
	vec3 specularColorComp = clamp(specularColor * specularPower * pow(cosCorr, specularTightness), 0, 1);

	vec3 diffuseColorComp = clamp(diffuseColor, 0, 1);
	
	// Start of True Reflection
	
	float fragDist = length(fsIn.position);
	float farPlane = 1.0;
	float zFrag = gl_FragCoord.z;
	float zProj = gl_FragCoord.z/gl_FragCoord.w/0.3;
	float reflectionDepth = texelFetch(texReflectionDepth, ivec2(gl_FragCoord.xy), 0).r;
		
	vec3 eyeReflection = reflect(vec3(0.0, 0.0 - fsIn.position.y, zProj - 0.0), fsIn.normal);

	vec3 rightVector = vec3(1.0, 0.0, 0.0);
	vec3 tangent = normalize(cross(eyeReflection, rightVector));
	vec3 underWaterRay = vec3(0.0, fsIn.position.y - fsIn.position.y, reflectionDepth - zProj);
	vec3 reflectedDirection = normalize(reflect(underWaterRay, tangent));

	//var4 should range from 0.0 close to camera and 0.09 far from camera
	float reflectionDepthSmall = clamp((1 - pow(reflectionDepth, 120) - 0.09), 0.0, 1.0);
	float divisor = 30.0 + 675.0 - reflectionDepthSmall*675;
	//can be 450 and 440 or 500 and 500
	float multiplicator = var4 + zFrag*var5 + zFrag*zFrag*var6;

	float reflectionDistance = (1 - abs(zProj - reflectionDepth)/divisor)*multiplicator;
	ivec2 reflectionTexCoord = ivec2(gl_FragCoord.xy + reflectedDirection.xy*reflectionDistance);
	float depthRatio = 1 - (abs(dmap_depth)/9 + 0.111);
	vec3 trueReflectionColor = clamp(depthRatio*texelFetch(texReflectionColor,  reflectionTexCoord, 0).rgb, 0.0, 1.0);

	if (reflectionTexCoord.x < 0 || reflectionTexCoord.x > screenWidth || reflectionTexCoord.y < 0 || reflectionTexCoord.y > screenHeight)
		trueReflectionColor = skyColor;
	
	// End of True Reflection


	float texColor = texture(texDisplacement, fsIn.textureCoord).r;
	vec3 texColorComp = vec3(texColor, texColor, texColor);

	float alphaComp = (1 - clamp(dot(fsIn.normal, normalize(fsIn.position)), 0, 1))/2;		//2
	float alpha2 = dot(fsIn.normal, normalize(fsIn.position))*var2;							//0.6

	alphaComp = max(alphaComp, alpha2);
	
	float zTexel = texelFetch(zBufferTexture, ivec2(gl_FragCoord.xy), 0).r;
	
	//Gives Sharp Lines around edges
	float cameraTransparency = 4;
	float cameraEdgeFactor = 0.015;
	float edgeFactor = var1;
	float zDifference = 1 - pow(zFrag/zTexel, cameraTransparency + fragDist*fragDist*cameraEdgeFactor);
	zDifference = clamp(round(sqrt(zDifference)*edgeFactor), 0, 1)/2 + 1.0;

	//Can see through shallow water
	//float zDifference = 1 - abs((zFrag/zTexel/(zFrag+zTexel) - var3))*var1;
	//zDifference = clamp(round(sqrt(zDifference)*var2), 0, 1)/2 + 0.5;

	float colorR = pow(texelFetch( texReflectionDepth, ivec2(gl_FragCoord.xy), 0).r, 20);
    vec3 reflectionColor = vec3(colorR, colorR, colorR);
	
	float worldDepthValue = texture( worldDepthMap, vec2(fsIn.worldPosition.x/2560 + 0.5, fsIn.worldPosition.z/2560 + 0.5), 0).r;
	float worldDepthAlpha = clamp(texture( worldDepthMap, vec2(fsIn.worldPosition.x/2560 + 0.5, fsIn.worldPosition.z/2560 + 0.5), 0).r, 0.7, 1.0);
	float worldDepthFoamValue = texture( worldDepthMapFoam, vec2(fsIn.worldPosition.x/2560 + 0.5, fsIn.worldPosition.z/2560 + 0.5), 0).r;


	float foamDmapDepth = abs(dmap_depth/10.0);

	float foamR = texture( foamTexture, fsIn.foamTextureCoord*20 + 0.5, 0).r;
	float foamAlpha = clamp(((1 - worldDepthFoamValue) - 0.52)*5, 0.0, 0.5);
	vec4 foamColor = vec4(vec3(foamR*foamAlpha*0.5, foamR*foamAlpha*0.7, foamR*foamAlpha), foamR*foamAlpha*foamDmapDepth);

	float foamR2 = texture( foamTexture, fsIn.foamTextureCoord2*20, 0).r;
	vec4 foamColor2 = vec4(vec3(foamR2*foamAlpha*0.5, foamR2*foamAlpha*0.7, foamR2*foamAlpha), foamR2*foamAlpha*foamDmapDepth/2);

	vec4 waterColor = vec4(trueReflectionColor*(1 - abs(dmap_depth/10)) + (specularColorComp + reflectionColorComp*(1 - foamAlpha*2) + diffuseColorComp)*(abs(dmap_depth/10)*0.8 + 0.2), zDifference*alphaComp*worldDepthAlpha);

	color = waterColor + foamColor + foamColor2;
}