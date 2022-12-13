#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

#include "PointLight.hlsl"

cbuffer ObjectCBuf
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
    bool useNormalMap;
    float normalMapWeight;
};

Texture2D tex;
Texture2D nmap : register(t2);

SamplerState splr;


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
    // replace normal with mapped if normal mapping enabled
    if (useNormalMap)
    {
        const float3 mappedNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, splr);
        viewNormal = lerp(viewNormal, mappedNormal, normalMapWeight);
    }
    
    // calculate lighting
    float3 diffuse = { 0.0f, 0.0f, 0.0f };
    float3 specular = { 0.0f, 0.0f, 0.0f };
    float3 ambient = { 0.0f, 0.0f, 0.0f };
    for (int i = 0; i < num_point_lights; i++)
    {
    // fragment to light vector data
        const LightVectorData lv = CalculateLightVectorData(pointLights[i].viewLightPos, viewFragPos);
    // attenuation
        const float att = Attenuate(pointLights[i].attConst, pointLights[i].attLin, pointLights[i].attQuad, lv.distToL);
    // diffuse
        diffuse += Diffuse(pointLights[i].diffuseColor, pointLights[i].diffuseIntensity, att, lv.dirToL, viewNormal);
    // specular
        specular += Speculate(
            pointLights[i].diffuseColor * pointLights[i].diffuseIntensity * specularColor, specularWeight, viewNormal,
        lv.vToL, viewFragPos, att, specularGloss
        );
    // ambient
        ambient += pointLights[i].ambient;
    }
    ambient /= num_point_lights;
    // final color
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}