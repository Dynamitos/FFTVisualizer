#version 450
layout(row_major) uniform;
layout(row_major) buffer;

#line 134 0
layout(binding = 3, set = 1)
uniform texture2D gDisplacementTexture_0;


#line 134
layout(binding = 4, set = 1)
uniform sampler gParallaxSampler_0;


#line 132
vec2 parallaxMapping_0(vec2 texCoords_0, vec3 viewDir_0)
{
    vec4 _S1 = (texture(sampler2D(gDisplacementTexture_0,gParallaxSampler_0), (texCoords_0)));

#line 134
    float height_0 = _S1.x;
    vec2 _S2 = viewDir_0.xy / viewDir_0.z;
    return texCoords_0 - _S2 * (height_0 * 0.10000000000000000555);
}

layout(binding = 1, set = 1)
uniform texture2D gDiffuseTexture_0;

layout(binding = 2, set = 1)
uniform texture2D gNormalTexture_0;

struct Light_0
{
    vec4 lightPos_0;
    vec4 lightColor_0;
    vec4 attenuation_0;
    float shineDamper_0;
    float reflectivity_0;
};


#line 153
layout(binding = 0, set = 1)
layout(std140) uniform _S3
{
    Light_0 _data;
} gLight_0;
layout(location = 0)
out vec4 _S4;

layout(location = 0)
in vec2 _S5;

layout(location = 1)
in vec3 _S6;

layout(location = 2)
in vec3 _S7;

layout(location = 3)
in vec3 _S8;

struct FragmentData_0
{
    vec2 texCoords_1;
    vec3 tangentLightPos_0;
    vec3 tangentViewPos_0;
    vec3 tangentFragPos_0;
};

struct FragmentShaderInput_0
{
    FragmentData_0 fragData_0;
};


#line 141
void main()
{
    FragmentShaderInput_0 _S9 = FragmentShaderInput_0(FragmentData_0(_S5, _S6, _S7, _S8));

#line 143
    vec3 viewDir_1 = normalize(_S9.fragData_0.tangentViewPos_0 - _S9.fragData_0.tangentFragPos_0);
    vec2 texCoords_2 = parallaxMapping_0(_S9.fragData_0.texCoords_1, viewDir_1);

    vec4 textureColor_0 = (texture(sampler2D(gDiffuseTexture_0,gParallaxSampler_0), (texCoords_2)));
    vec4 _S10 = (texture(sampler2D(gNormalTexture_0,gParallaxSampler_0), (texCoords_2)));
    vec3 unitNormal_0 = normalize(_S10.xyz * 2.00000000000000000000 - 1.00000000000000000000);


    vec3 toLightVector_0 = _S9.fragData_0.tangentLightPos_0 - _S9.fragData_0.tangentFragPos_0;
    float dist_0 = length(toLightVector_0);
    float attFactor_0 = gLight_0._data.attenuation_0.x + gLight_0._data.attenuation_0.y * dist_0 + gLight_0._data.attenuation_0.z * (dist_0 * dist_0);

    vec3 unitLightVector_0 = normalize(toLightVector_0);
    float nDotl_0 = dot(unitNormal_0, unitLightVector_0);
    float brightness_0 = max(nDotl_0, 0.20000000000000001110);

    vec3 reflectedLightDirection_0 = reflect(-unitLightVector_0, unitNormal_0);
    float specularFactor_0 = dot(reflectedLightDirection_0, viewDir_1);
    float specularFactor_1 = max(specularFactor_0, 0.00000000000000000000);
    float dampedFactor_0 = pow(specularFactor_1, gLight_0._data.shineDamper_0);

    vec3 totalDiffuse_0 = brightness_0 * gLight_0._data.lightColor_0.xyz / attFactor_0;
    vec3 totalSpecular_0 = dampedFactor_0 * gLight_0._data.reflectivity_0 * gLight_0._data.lightColor_0.xyz / attFactor_0;

    vec3 totalDiffuse_1 = max(totalDiffuse_0, vec3(0.20000000000000001110));
    _S4 = vec4(totalDiffuse_1, 1.00000000000000000000) * textureColor_0 + vec4(totalSpecular_0, 1.00000000000000000000);
    return;
}

