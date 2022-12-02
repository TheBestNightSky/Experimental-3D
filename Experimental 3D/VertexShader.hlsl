struct VS_INPUT {
	float2 pos : POSITION;
	float3 color : COLOR;
};

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float3 color : COLOR;
};

cbuffer Cbuf {
	matrix transform;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.pos = mul(float4(input.pos, 0.0f, 1.0f), transform);
	output.color = input.color;
	return output;
}