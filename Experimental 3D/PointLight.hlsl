//cbuffer PointLightCBuf
//{
//    float3 viewLightPos;
//    float3 ambient;
//    float3 diffuseColor;
//    float diffuseIntensity;
//    float attConst;
//    float attLin;
//    float attQuad;
//};

struct PointLight {
    float3 LightPos;
    float pad0;
    float3 ambient;
    float pad1;
    float3 diffuseColor;
    float pad2;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

StructuredBuffer<PointLight> pointLights : register(t4);

cbuffer LightCBuf : register(b2)
{
    matrix CameraView;
};