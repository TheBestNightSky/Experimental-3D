#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

cbuffer ObjectCBuf
{
	float specularIntensity;
	float specularPower;
	bool normalMapEnabled;
	float padding[1];
};

#include "Transform.hlsl"
#include "PointLight.hlsl"

Texture2D tex;
Texture2D nmap : register(t2);

SamplerState splr;


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_Target
{
	// sample normal from map if normal mapping enabled
	if (normalMapEnabled)
	{
		// sample and unpack normal data
		const float3 normalSample = nmap.Sample(splr, tc).xyz;
		float3 tanNormal;
		const float3 objectNormal = normalSample * 2.0f - 1.0f;
		viewNormal = normalize(mul(objectNormal, (float3x3) modelView));
	}
	
	// calculate lighting
	float3 diffuse = {0.0f, 0.0f, 0.0f};
	float3 specular = { 0.0f, 0.0f, 0.0f };
	float3 ambient = { 0.0f, 0.0f, 0.0f };
    uint num_point_lights;
    uint stride;
    pointLights.GetDimensions(num_point_lights, stride);
	for (int i = 0; i < num_point_lights; i++) {
		//bring light pos into view space
        float3 viewLightPos = (float3) mul(float4(pointLights[i].LightPos, 1), CameraView);
		// fragment to light vector data
		const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
		// attenuation
		const float att = Attenuate(pointLights[i].attConst, pointLights[i].attLin, pointLights[i].attQuad, lv.distToL);
		// diffuse intensity
		diffuse += Diffuse(pointLights[i].diffuseColor, pointLights[i].diffuseIntensity, att, lv.dirToL, viewNormal);
		// specular
		specular += Speculate(
			specularIntensity.rrr, 1.0f, viewNormal, lv.vToL,
			viewFragPos, att, specularPower
		);
		// ambient
        ambient += pointLights[i].ambient * att;

	}
    ambient /= num_point_lights;
	// final color
	return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}