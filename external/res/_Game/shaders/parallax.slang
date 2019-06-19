
float4x4 inverse(float4x4 m) {
  float
      a00 = m[0][0], a01 = m[0][1], a02 = m[0][2], a03 = m[0][3],
      a10 = m[1][0], a11 = m[1][1], a12 = m[1][2], a13 = m[1][3],
      a20 = m[2][0], a21 = m[2][1], a22 = m[2][2], a23 = m[2][3],
      a30 = m[3][0], a31 = m[3][1], a32 = m[3][2], a33 = m[3][3],

      b00 = a00 * a11 - a01 * a10,
      b01 = a00 * a12 - a02 * a10,
      b02 = a00 * a13 - a03 * a10,
      b03 = a01 * a12 - a02 * a11,
      b04 = a01 * a13 - a03 * a11,
      b05 = a02 * a13 - a03 * a12,
      b06 = a20 * a31 - a21 * a30,
      b07 = a20 * a32 - a22 * a30,
      b08 = a20 * a33 - a23 * a30,
      b09 = a21 * a32 - a22 * a31,
      b10 = a21 * a33 - a23 * a31,
      b11 = a22 * a33 - a23 * a32,

      det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

  return float4x4(
      a11 * b11 - a12 * b10 + a13 * b09,
      a02 * b10 - a01 * b11 - a03 * b09,
      a31 * b05 - a32 * b04 + a33 * b03,
      a22 * b04 - a21 * b05 - a23 * b03,
      a12 * b08 - a10 * b11 - a13 * b07,
      a00 * b11 - a02 * b08 + a03 * b07,
      a32 * b02 - a30 * b05 - a33 * b01,
      a20 * b05 - a22 * b02 + a23 * b01,
      a10 * b10 - a11 * b08 + a13 * b06,
      a01 * b08 - a00 * b10 - a03 * b06,
      a30 * b04 - a31 * b02 + a33 * b00,
      a21 * b02 - a20 * b04 - a23 * b00,
      a11 * b07 - a10 * b09 - a12 * b06,
      a00 * b09 - a01 * b07 + a02 * b06,
      a31 * b01 - a30 * b03 - a32 * b00,
      a20 * b03 - a21 * b01 + a22 * b00) / det;
}


struct ViewParams
{
	float4x4 projectionMatrix;
	float4x4 viewMatrix;
	float4x4 modelMatrix;
	float4 lightPos;
};

layout(set = 0, binding = 0)
ParameterBlock<ViewParams> gViewParams;

struct AssembledVertex
{
	float3 position;
	float2 texCoords;
	float3 normal;
	float3 tangent;
	float3 bitangent;
};

struct FragmentData
{
	float2 texCoords;
	float3 tangentLightPos;
	float3 tangentViewPos;
	float3 tangentFragPos;
};

struct VertexShaderInput
{
	AssembledVertex assembledVertex : A;
};

struct VertexShaderOutput
{
	FragmentData fragData : FragIn;
	float4 sv_position : SV_Position;
};

[shader("vertex")]
VertexShaderOutput vertexMain(VertexShaderInput input)
{
	VertexShaderOutput output;

	float4 worldPos = mul(gViewParams.modelMatrix, float4(input.assembledVertex.position, 1));
	float4 viewPos = mul(gViewParams.viewMatrix, worldPos);
	output.sv_position = mul(gViewParams.projectionMatrix, viewPos);
	output.fragData.texCoords = input.assembledVertex.texCoords;
	float3 t = normalize(mul(float3x3(gViewParams.modelMatrix), input.assembledVertex.tangent));
	float3 b = normalize(mul(float3x3(gViewParams.modelMatrix), input.assembledVertex.bitangent));
	float3 n = normalize(mul(float3x3(gViewParams.modelMatrix), input.assembledVertex.normal));

	float3x3 tbn = transpose(float3x3(t,b,n));

	output.fragData.tangentLightPos = mul(tbn, gViewParams.lightPos.xyz);
	output.fragData.tangentViewPos = mul(tbn, (worldPos - (mul(inverse(gViewParams.viewMatrix), float4(0,0,0,1)))).xyz);
	output.fragData.tangentFragPos = mul(tbn, worldPos.xyz);

	return output;
}

struct FragmentShaderInput
{
	FragmentData fragData : FragIn;
};


struct Light
{
	float4 lightPos;
	float4 lightColor;
	float4 attenuation;
	float shineDamper;
	float reflectivity;
};
layout(set = 1, binding = 0)
ParameterBlock<Light> gLight;
layout(set = 1, binding = 1)
Texture2D<float4> gDiffuseTexture;
layout(set = 1, binding = 2)
Texture2D<float4> gNormalTexture;
layout(set = 1, binding = 3)
Texture2D<float4> gDisplacementTexture;

layout(set = 1, binding = 4)
SamplerState gParallaxSampler;


float2 parallaxMapping(float2 texCoords, float3 viewDir)
{
	float height = gDisplacementTexture.Sample(gParallaxSampler, texCoords).r;
	float2 p = viewDir.xy / viewDir.z * (height * 0.1f);
	return texCoords - p;
}


[shader("fragment")]
float4 fragmentMain(FragmentShaderInput input) : SV_Target
{
	float3 viewDir = normalize(input.fragData.tangentViewPos - input.fragData.tangentFragPos);
	float2 texCoords = parallaxMapping(input.fragData.texCoords, viewDir);
	
	float4 textureColor = gDiffuseTexture.Sample(gParallaxSampler, texCoords);
	float3 unitNormal = gNormalTexture.Sample(gParallaxSampler, texCoords).xyz;
	unitNormal = normalize(unitNormal * 2.0f - 1.0f);

	float3 unitVectorToCamera = viewDir;
	float3 toLightVector = input.fragData.tangentLightPos - input.fragData.tangentFragPos;
	float dist = length(toLightVector);
	float attFactor = gLight.attenuation.x + (gLight.attenuation.y * dist) + (gLight.attenuation.z * (dist * dist));
	
	float3 unitLightVector = normalize(toLightVector);
	float nDotl = dot(unitNormal, unitLightVector);
	float brightness = max(nDotl, 0.2f);
	float3 lightDirection = -unitLightVector;
	float3 reflectedLightDirection = reflect(lightDirection, unitNormal);
	float specularFactor = dot(reflectedLightDirection, unitVectorToCamera);
	specularFactor = max(specularFactor, 0.0f);
	float dampedFactor = pow(specularFactor, gLight.shineDamper);

	float3 totalDiffuse = (brightness * gLight.lightColor.xyz) / attFactor;
	float3 totalSpecular = (dampedFactor * gLight.reflectivity * gLight.lightColor.xyz) / attFactor;

	totalDiffuse = max(totalDiffuse, 0.2f);
	return float4(totalDiffuse, 1.0f) * textureColor + float4(totalSpecular, 1.0f); 
};