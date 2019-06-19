#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 texCoords_FS_in;
layout(location = 1) in vec3 tangentLightPos_FS_in;
layout(location = 2) in vec3 tangentViewPos_FS_in;
layout(location = 3) in vec3 tangentFragPos_FS_in;

layout(location = 0) out vec4 out_Color;

layout(set = 1, binding = 0) uniform Light {
	vec4 lightPos;
	vec4 lightColor;
	vec4 attenuation;
	float shineDamper;
	float reflectivity;
} light;
layout(set = 1, binding = 1) uniform sampler2D diffuseSampler;
layout(set = 1, binding = 2) uniform sampler2D normalSampler;
layout(set = 1, binding = 3) uniform sampler2D displacementSampler;

const float height_scale = 0.1f;


vec2 parallaxMapping(vec2 texCoords, vec3 viewDir)
{
	float height = texture(displacementSampler, texCoords).r;
	vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
	return texCoords - p;
}

void main()
{
	vec3 viewDir = normalize(tangentViewPos_FS_in - tangentFragPos_FS_in);
	vec2 texCoords = parallaxMapping(texCoords_FS_in, viewDir);
	
	vec4 textureColor = texture(diffuseSampler, texCoords);
	vec3 unitNormal = texture(normalSampler, texCoords).xyz;
	unitNormal = normalize(unitNormal * 2.0f - 1.0);
	//vec4 specInfo = texture(specularSampler, texCoords);

	
	vec3 unitVectorToCamera = viewDir;//cameraPos_FS_in - worldPos_FS_in;
	vec3 toLightVector = tangentLightPos_FS_in - tangentFragPos_FS_in;
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
	
	//totalSpecular *= specInfo.r;
	
	totalDiffuse = max(totalDiffuse, 0.2);
	out_Color = vec4(totalDiffuse, 1.0f) * textureColor + vec4(totalSpecular, 1.0f);
}