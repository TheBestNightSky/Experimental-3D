#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

#include "PointLight.hlsl"

cbuffer ObjectCBuf
{
    bool useGlossAlpha;
    bool useSpecularMap;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

Texture2D tex;
Texture2D spec;

SamplerState splr;


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_Target
{
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
    
    // specular parameters
    float specularPowerLoaded = specularGloss;
    const float4 specularSample = spec.Sample(splr, tc);
    float3 specularReflectionColor;
    if (useSpecularMap)
    {
        specularReflectionColor = specularSample.rgb;
    }
    else
    {
        specularReflectionColor = specularColor;
    }
    if (useGlossAlpha)
    {
        specularPowerLoaded = pow(2.0f, specularSample.a * 13.0f);
    }
    
    //calculate lighting
    float3 diffuse = { 0.0f, 0.0f, 0.0f };
    float3 specularReflected = { 0.0f, 0.0f, 0.0f };
    float3 ambient = { 0.0f, 0.0f, 0.0f };
    for (int i = 0; i < num_point_lights; i++)
    {
    // fragment to light vector data
        const LightVectorData lv = CalculateLightVectorData(pointLights[i].viewLightPos, viewFragPos);
    
    // attenuation
        const float att = Attenuate(pointLights[i].attConst, pointLights[i].attLin, pointLights[i].attQuad, lv.distToL);
    // diffuse light
        diffuse += Diffuse(pointLights[i].diffuseColor, pointLights[i].diffuseIntensity, att, lv.dirToL, viewNormal);
    // specular reflected
        specularReflected += Speculate(
            pointLights[i].diffuseColor * specularReflectionColor, specularWeight, viewNormal,
            lv.vToL, viewFragPos, att, specularPowerLoaded
        );
    // ambient
        ambient += pointLights[i].ambient;
    }
    ambient /= num_point_lights;
    // final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specularReflected), 1.0f);
}