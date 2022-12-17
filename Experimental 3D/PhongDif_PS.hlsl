#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

cbuffer ObjectCBuf : register(b1)
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

#include "PointLight.hlsl"

Texture2D tex;

SamplerState splr;

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_Target
{
	// renormalize interpolated normal
	viewNormal = normalize(viewNormal);
    // calculate lighting
    float3 diffuse = { 0.0f, 0.0f, 0.0f };
    float3 specular = { 0.0f, 0.0f, 0.0f };
    float3 ambient = { 0.0f, 0.0f, 0.0f };
    uint num_point_lights;
    uint stride;
    pointLights.GetDimensions(num_point_lights, stride);
    for (int i = 0; i < num_point_lights; i++)
    {
        //bring light pos into view space
        float3 viewLightPos = (float3) mul(float4(pointLights[i].LightPos, 1), CameraView);
	    // fragment to light vector data
        const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
	    // diffuse attenuation
        const float att = Attenuate(pointLights[i].attConst, pointLights[i].attLin, pointLights[i].attQuad, lv.distToL);
	    // diffuse
        diffuse += Diffuse(pointLights[i].diffuseColor, pointLights[i].diffuseIntensity, att, lv.dirToL, viewNormal);
	    // specular
        specular += Speculate(pointLights[i].diffuseColor * pointLights[i].diffuseIntensity * specularColor, specularWeight, viewNormal, lv.vToL, viewFragPos, att, specularGloss);
	    //ambient
        ambient += pointLights[i].ambient * att;
    }
    ambient /= num_point_lights;
	// final color
	return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}