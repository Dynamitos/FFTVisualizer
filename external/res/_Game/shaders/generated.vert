#version 450
layout(row_major) uniform;
layout(row_major) buffer;

#line 77 0
struct ViewParams_0
{
    mat4x4 projectionMatrix_0;
    mat4x4 viewMatrix_0;
    mat4x4 modelMatrix_0;
    vec4 lightPos_0;
};




layout(binding = 0)
layout(std140) uniform _S1
{
    ViewParams_0 _data;
} gViewParams_0;

#line 2
mat4x4 inverse_0(mat4x4 m_0)
{

#line 2
    float a00_0 = m_0[0][0];
    float a01_0 = m_0[0][1];
    float a02_0 = m_0[0][2];
    float a03_0 = m_0[0][3];
    float a10_0 = m_0[1][0];
    float a11_0 = m_0[1][1];
    float a12_0 = m_0[1][2];
    float a13_0 = m_0[1][3];
    float a20_0 = m_0[2][0];
    float a21_0 = m_0[2][1];
    float a22_0 = m_0[2][2];
    float a23_0 = m_0[2][3];
    float a30_0 = m_0[3][0];
    float a31_0 = m_0[3][1];
    float a32_0 = m_0[3][2];
    float a33_0 = m_0[3][3];

#line 9
    float b00_0 = a00_0 * a11_0 - a01_0 * a10_0;
    float b01_0 = a00_0 * a12_0 - a02_0 * a10_0;
    float b02_0 = a00_0 * a13_0 - a03_0 * a10_0;
    float b03_0 = a01_0 * a12_0 - a02_0 * a11_0;
    float b04_0 = a01_0 * a13_0 - a03_0 * a11_0;
    float b05_0 = a02_0 * a13_0 - a03_0 * a12_0;
    float b06_0 = a20_0 * a31_0 - a21_0 * a30_0;
    float b07_0 = a20_0 * a32_0 - a22_0 * a30_0;
    float b08_0 = a20_0 * a33_0 - a23_0 * a30_0;
    float b09_0 = a21_0 * a32_0 - a22_0 * a31_0;
    float b10_0 = a21_0 * a33_0 - a23_0 * a31_0;
    float b11_0 = a22_0 * a33_0 - a23_0 * a32_0;

#line 40
    mat4x4 _S2 = mat4x4(a11_0 * b11_0 - a12_0 * b10_0 + a13_0 * b09_0, a02_0 * b10_0 - a01_0 * b11_0 - a03_0 * b09_0, a31_0 * b05_0 - a32_0 * b04_0 + a33_0 * b03_0, a22_0 * b04_0 - a21_0 * b05_0 - a23_0 * b03_0, a12_0 * b08_0 - a10_0 * b11_0 - a13_0 * b07_0, a00_0 * b11_0 - a02_0 * b08_0 + a03_0 * b07_0, a32_0 * b02_0 - a30_0 * b05_0 - a33_0 * b01_0, a20_0 * b05_0 - a22_0 * b02_0 + a23_0 * b01_0, a10_0 * b10_0 - a11_0 * b08_0 + a13_0 * b06_0, a01_0 * b08_0 - a00_0 * b10_0 - a03_0 * b06_0, a30_0 * b04_0 - a31_0 * b02_0 + a33_0 * b00_0, a21_0 * b02_0 - a20_0 * b04_0 - a23_0 * b00_0, a11_0 * b07_0 - a10_0 * b09_0 - a12_0 * b06_0, a00_0 * b09_0 - a01_0 * b07_0 + a02_0 * b06_0, a31_0 * b01_0 - a30_0 * b03_0 - a32_0 * b00_0, a20_0 * b03_0 - a21_0 * b01_0 + a22_0 * b00_0) / (b00_0 * b11_0 - b01_0 * b10_0 + b02_0 * b09_0 + b03_0 * b08_0 - b04_0 * b07_0 + b05_0 * b06_0);

#line 24
    return _S2;
}


#line 55
layout(location = 0)
out vec2 _S3;


#line 55
layout(location = 1)
out vec3 _S4;

layout(location = 2)
out vec3 _S5;

layout(location = 3)
out vec3 _S6;

layout(location = 0)
in vec3 _S7;

layout(location = 1)
in vec2 _S8;

layout(location = 2)
in vec3 _S9;

layout(location = 3)
in vec3 _S10;

layout(location = 4)
in vec3 _S11;

struct AssembledVertex_0
{
    vec3 position_0;
    vec2 texCoords_0;
    vec3 normal_0;
    vec3 tangent_0;
    vec3 bitangent_0;
};

struct VertexShaderInput_0
{
    AssembledVertex_0 assembledVertex_0;
};

struct FragmentData_0
{
    vec2 texCoords_1;
    vec3 tangentLightPos_0;
    vec3 tangentViewPos_0;
    vec3 tangentFragPos_0;
};

struct VertexShaderOutput_0
{
    FragmentData_0 fragData_0;
    vec4 sv_position_0;
};


#line 84
void main()
{
    VertexShaderInput_0 _S12 = VertexShaderInput_0(AssembledVertex_0(_S7, _S8, _S9, _S10, _S11));

#line 86
    VertexShaderOutput_0 output_0;

    vec4 worldPos_0 = vec4(_S12.assembledVertex_0.position_0, float(1)) * gViewParams_0._data.modelMatrix_0;

    output_0.sv_position_0 = worldPos_0 * gViewParams_0._data.viewMatrix_0 * gViewParams_0._data.projectionMatrix_0;
    output_0.fragData_0.texCoords_1 = _S12.assembledVertex_0.texCoords_0;
    vec3 t_0 = normalize(_S12.assembledVertex_0.tangent_0 * mat3x3(gViewParams_0._data.modelMatrix_0));
    vec3 b_0 = normalize(_S12.assembledVertex_0.bitangent_0 * mat3x3(gViewParams_0._data.modelMatrix_0));
    vec3 n_0 = normalize(_S12.assembledVertex_0.normal_0 * mat3x3(gViewParams_0._data.modelMatrix_0));

    mat3x3 tbn_0 = transpose(mat3x3(t_0, b_0, n_0));

    output_0.fragData_0.tangentLightPos_0 = gViewParams_0._data.lightPos_0.xyz * tbn_0;
    mat4x4 _S13 = inverse_0(gViewParams_0._data.viewMatrix_0);

#line 99
    output_0.fragData_0.tangentViewPos_0 = (worldPos_0 - vec4(float(0), float(0), float(0), float(1)) * _S13).xyz * tbn_0;
    output_0.fragData_0.tangentFragPos_0 = worldPos_0.xyz * tbn_0;

    VertexShaderOutput_0 _S14 = output_0;
    FragmentData_0 _S15 = _S14.fragData_0;
    _S3 = _S15.texCoords_1;
    _S4 = _S15.tangentLightPos_0;
    _S5 = _S15.tangentViewPos_0;
    _S6 = _S15.tangentFragPos_0;
    gl_Position = _S14.sv_position_0;
    return;
}

