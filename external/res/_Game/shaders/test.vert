#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform MVP {
	mat4 projection;
	mat4 view;
	mat4 model;
} pushConsts;

layout(location = 0) in vec3 position_VS_in;
layout(location = 1) in vec2 texCoords_VS_in;
layout(location = 2) in vec3 normal_VS_in;
layout(location = 3) in vec3 tangent_VS_in;
layout(location = 4) in vec3 bitangent_VS_in;

layout(location = 0) out vec3 worldPos_FS_in;
layout(location = 1) out vec2 texCoords_FS_in;
layout(location = 2) out vec3 normal_FS_in;
layout(location = 3) out vec3 cameraPos_FS_in;

void main()
{
	vec4 worldPos = pushConsts.model * vec4(position_VS_in, 1);
	gl_Position = pushConsts.projection * pushConsts.view * worldPos;
	worldPos_FS_in = worldPos.xyz;
	texCoords_FS_in = texCoords_VS_in;
	normal_FS_in = (pushConsts.model * vec4(normal_VS_in, 1)).xyz;
	cameraPos_FS_in = worldPos.xyz - (inverse(pushConsts.view) * vec4(0,0,0,1)).xyz;
}