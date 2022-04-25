#version 450

out vec3 position_FS_in;
out vec2 texCoords_FS_in;

void main()
{
	texCoords_FS_in = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
	position_FS_in = vec3(texCoords_FS_in.xy * 2.0f - 1.0f, 0.0f);
	position_FS_in.y *= -1f;
	gl_Position = vec4(position_FS_in, 1);
}