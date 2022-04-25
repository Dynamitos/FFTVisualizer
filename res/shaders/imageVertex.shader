#version 450

out vec2 texCoords_FS_in;
out vec3 worldPos_FS_in;

uniform mat4 transformationMatrix;
uniform mat4 viewMatrix;
uniform mat4 projection;
uniform float intensity;

void main(void)
{
	texCoords_FS_in = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
	vec4 position = vec4(texCoords_FS_in * 2.0f + -1.0f, 0.0f, 1.0f);
	position.xy = position.xy * -1;
	vec4 worldPos = transformationMatrix * position;
	worldPos_FS_in = worldPos.xyz;
	gl_Position = viewMatrix * worldPos;
}