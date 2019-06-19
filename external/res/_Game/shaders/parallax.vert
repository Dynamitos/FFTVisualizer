#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(set = 0, binding = 0) uniform MVP {
	mat4 projection;
	mat4 view;
	mat4 model;
	vec4 lightPos;
} pushConsts;

layout(location = 0) in vec3 position_VS_in;
layout(location = 1) in vec2 texCoords_VS_in;
layout(location = 2) in vec3 normal_VS_in;
layout(location = 3) in vec3 tangent_VS_in;
layout(location = 4) in vec3 bitangent_VS_in;

layout(location = 0) out vec2 texCoords_FS_in;
layout(location = 1) out vec3 tangentLightPos_FS_in;
layout(location = 2) out vec3 tangentViewPos_FS_in;
layout(location = 3) out vec3 tangentFragPos_FS_in;



void main()
{
	vec4 worldPos = pushConsts.model * vec4(position_VS_in, 1);
	gl_Position = pushConsts.projection * pushConsts.view * worldPos;
	texCoords_FS_in = texCoords_VS_in;
	vec3 t = normalize(mat3(pushConsts.model) * tangent_VS_in);
	vec3 b = normalize(mat3(pushConsts.model) * bitangent_VS_in);
	vec3 n = normalize(mat3(pushConsts.model) * normal_VS_in);
	mat3 tbn = transpose(mat3(t,b,n));
	
	tangentLightPos_FS_in = tbn * pushConsts.lightPos.xyz;
	tangentViewPos_FS_in = tbn * (worldPos - (inverse(pushConsts.view) * vec4(0,0,0,1))).xyz;
	tangentFragPos_FS_in = tbn * worldPos.xyz;	
}