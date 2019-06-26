#version 440

layout(location = 0) in vec2 vertex_VS_in;
layout(location = 1) in vec4 positionScale_VS_in;
layout(location = 2) in vec4 rotation_VS_in;


layout(location = 0) out vec2 texCoords_FS_in;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main(void)
{
	texCoords_FS_in = vertex_VS_in + vec2(0.5, 0.5);
    mat4 mx, my, mz;

	// rotate around x
	float s = sin(rotation_VS_in.x);
	float c = cos(rotation_VS_in.x);

	mx[0] = vec4(1, 0, 0, 0);
	mx[1] = vec4(0, c, s, 0);
	mx[2] = vec4(0, -s, c, 0);
    mx[3] = vec4(0, 0, 0, 1);

	// rotate around y
	s = sin(rotation_VS_in.y);
	c = cos(rotation_VS_in.y);

	my[0] = vec4(c, 0, -s, 0);
    my[1] = vec4(0, 1, 0, 0);
    my[2] = vec4(s, 0, c, 0);
    my[3] = vec4(0, 0, 0, 1);

	// rot around z
	s = sin(rotation_VS_in.z);
	c = cos(rotation_VS_in.z);

	mz[0] = vec4(c, s, 0, 0);
    mz[1] = vec4(-s, c, 0, 0);
    mz[2] = vec4(0, 0, 1, 0);
    mz[3] = vec4(0, 0, 0, 1);

    mat4 rotMat = mz * my * mx;
    mat4 scaleMat;
	float scale = positionScale_VS_in.w;
    scaleMat[0] = vec4(scale, 0, 0, 0);
    scaleMat[1] = vec4(0, scale, 0, 0);
    scaleMat[2] = vec4(0, 0, scale, 0);
    scaleMat[3] = vec4(0, 0, 0, 1);
    mat4 posMat;
    posMat[0] = vec4(1, 0, 0, 0);
    posMat[1] = vec4(0, 1, 0, 0);
    posMat[2] = vec4(0, 0, 1, 0);
    posMat[3] = vec4(positionScale_VS_in.x, positionScale_VS_in.y, positionScale_VS_in.z, 1);

    vec4 worldPos = posMat */* rotMat **/ scaleMat * vec4(vertex_VS_in, 0, 1);
	gl_Position = projectionMatrix * viewMatrix * worldPos;
}