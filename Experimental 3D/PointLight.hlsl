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
    float3 viewLightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer PointLightCBuf {
    int num_point_lights;
    uniform PointLight pointLights[1000];
};