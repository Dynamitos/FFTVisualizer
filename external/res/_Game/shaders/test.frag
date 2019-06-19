#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 worldPos_FS_in;
layout(location = 1) in vec2 texCoords_FS_in;
layout(location = 2) in vec3 normal_FS_in;
layout(location = 3) in vec3 cameraPos_FS_in;

layout(location = 0) out vec4 out_Color;

layout(std140, set = 1, binding = 0) uniform Light {
	vec4 lightPos;
	vec4 lightColor;
	vec4 attenuation;
	float shineDamper;
	float reflectivity;
} light;
layout(set = 1, binding = 1) uniform sampler2D diffuseSampler;
layout(set = 1, binding = 2) uniform sampler2D normalSampler;
layout(set = 1, binding = 3) uniform sampler2D specularSampler;
layout(set = 1, binding = 4) uniform sampler2D glowSampler;



void main(void)
{
	vec3 actualNormal = normal_FS_in;
	
	vec3 unitNormal = normalize(actualNormal);
	vec3 unitVectorToCamera = cameraPos_FS_in - worldPos_FS_in;
	vec3 toLightVector = light.lightPos.xyz - worldPos_FS_in;
	float distance = length(toLightVector);
	float attFactor = light.attenuation.x + (light.attenuation.y * distance) + (light.attenuation.z * (distance * distance));
	
	vec3 unitLightVector = normalize(toLightVector);
	float nDotl = dot(unitNormal, unitLightVector);
	float brightness = max(nDotl, 0.2);
	vec3 lightDirection = -unitLightVector;
	vec3 reflectedLightDirection = reflect(lightDirection, unitNormal);
	float specularFactor = dot(reflectedLightDirection, unitVectorToCamera);
	specularFactor = max(specularFactor, 0.0f);
	float dampedFactor = pow(specularFactor, light.shineDamper);
	
	vec3 totalDiffuse = (brightness * light.lightColor.xyz) / attFactor;
	vec3 totalSpecular = (dampedFactor * light.reflectivity * light.lightColor.xyz) / attFactor;
	
	vec4 textureColor = texture(diffuseSampler, texCoords_FS_in);
	vec4 specInfo = texture(specularSampler, texCoords_FS_in);
	totalSpecular *= specInfo.r;
	
	totalDiffuse = max(totalDiffuse, 0.2);
	out_Color = vec4(totalDiffuse, 1.0f) * textureColor + vec4(totalSpecular, 1.0f);
}