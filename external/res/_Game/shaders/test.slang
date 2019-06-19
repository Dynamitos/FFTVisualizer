struct InputGeometry
{
	float3 position;
	float2 texCoord;
	float3 normal;
};

interface IBRDF
{
	float3 evaluate(float3 wo, float3 wi, float3 N);
};

struct BlinnPhong : IBRDF
{
	float3 kd;
	float3 ks;
	float specularity;
	
	float3 evaluate(float3 v, float3 l, float3 n)
	{
		float nDotL = saturate(dot(n, l));
		float3 h = normalize(l + v);
		float nDotH = saturate(dot(n, h));
		
		return kd*nDotL + ks*pow(nDotH, specularity);
	}
};

interface IMaterial
{
	associatedtype BRDF : IBRDF;
	
	BRDF prepare(InputGeometry geometry);
}

struct SimpleMaterial : IMaterial
{
	float3 diffuseColor;
	float3 specularColor;
	float specularity;
	
	typedef BlinnPhong BRDF;
	
	BlinnPhong prepare(InputGeometry input)
	{
		BlinnPhong brdf;
		brdf.kd = diffuseColor;
		brdf.ks = specularColor;
		brdf.specularity = specularity;
		return brdf;
	}
};

interface ILightEnv
{
	float3 illuminate<B:IBRDF>(InputGeometry input, B brdf, float3 wo);
};

struct DirectionalLight : ILightEnv
{
	float3 direction;
	float3 intensity;
	
	float3 illuminate<B:IBRDF>(InputGeometry input, B brdf, float3 wo)
	{
		return intensity * brdf.evaluate(wo, direction, input.normal);	
	}
};

struct PointLight : ILightEnv
{
	float3 position;
	float3 intensity;
	
	float3 illuminate<B:IBRDF>(InputGeometry input, B brdf, float3 wo)
	{
		float3 delta = position - input.position;
		float d = length(delta);
		float3 direction = normalize(delta);
		float3 illuminance = intensity / (d*d);
		return illuminance * brdf.evaluate(wo, direction, input.normal);
	}
};

struct LightArray<L : ILightEnv, let N : int> : ILightEnv
{
	L lights[N];
	
	float3 illuminate<B:IBRDF>(InputGeometry input, B brdf, float3 wo)
	{
		float3 sum = 0;
		for(int ii = 0; ii < N; ++ii)
		{
			sum += lights[ii].illuminate(input, brdf, wo);
		}
		return sum;
	}
};

struct LightPair<T : ILightEnv, U : ILightEnv> : ILightEnv
{
	T first;
	U second;
	
	float3 illuminate<B:IBRDF>(InputGeometry input, B brdf, float3 wo)
	{
		return first.illuminate(input, brdf, wo) +
			  second.illuminate(input, brdf, wo);
	}
};

struct EmptyLightEnv : ILightEnv
{
	float3 illuminate<B:IBRDF>(InputGeometry input, B brdf, float3 wo)
	{
		return 0;
	}
};

struct PerView
{
	float4x4 projectionMatrix;
	float4x4 viewMatrix;
	float3 eyePosition;
};

ParameterBlock<PerView> gViewParams;

struct PerModel
{
	float4x4 modelTransform;
	float4x4 inverseTransposeModelTransform;
};

ParameterBlock<PerModel> gModelParams;

type_param TLightEnv : ILightEnv;

ParameterBlock<TLightEnv> gLightEnv;

type_param TMaterial : IMaterial;
ParameterBlock<TMaterial> gMaterial;

struct AssembledVertex
{
	float3 position : POSITION;
	float2 texCoord : UV;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 biTangent : BITANGENT;
};

struct CoarseVertex
{
    float3 worldPosition;
    float3 worldNormal;
    float2 uv;
};

struct VertexStageOutput
{
	CoarseVertex coarseVertex : CoarseVertex;
	float4 sv_position : SV_Position;
};

[shader("vertex")]
VertexStageOutput vertexMain(
	AssembledVertex assembledVertex)
{
	VertexStageOutput output;
	
	float3 position = assembledVertex.position;
	float2 texCoord = assembledVertex.texCoord;
	float3 normal = assembledVertex.normal;
	float3 tangent = assembledVertex.tangent;
	float3 biTangent = assembledVertex.biTangent;
	
	float3 worldPosition = mul(gModelParams.modelTransform, float4(position, 1.0)).xyz;
	float3 worldNormal = mul(gModelParams.inverseTransposeModelTransform, float4(normal, 0.0)).xyz;
	
	output.coarseVertex.worldPosition = worldPosition;
    output.coarseVertex.worldNormal   = worldNormal;
    output.coarseVertex.uv            = texCoord;

	float4 viewPos = mul(gViewParams.viewMatrix, float4(worldPosition, 1.0));
	output.sv_position = mul(gViewParams.projectionMatrix, viewPos);

	return output;
}

[shader("fragment")]
float4 fragmentMain(CoarseVertex coarseVertex : CoarseVertex) : SV_Target
{
	InputGeometry input;
	input.position = coarseVertex.worldPosition;
	input.normal = normalize(coarseVertex.worldNormal);
	input.texCoord = coarseVertex.uv;
	
	float3 V = normalize(gViewParams.eyePosition - input.position);
	
	TMaterial.BRDF brdf = gMaterial.prepare(input);
	
	float3 color = gLightEnv.illuminate(input, brdf, V);
	
	return float4(color, 1);
}
