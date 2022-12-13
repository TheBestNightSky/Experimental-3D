#pragma once
#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffers.h"
#include "ConditionalNoexcept.h"
#include <vector>
#include <memory>

class PointLight
{
public:
	PointLight(Graphics& gfx, float radius = 0.5f);
	void SpawnControlWindow(const char* name = "Light") noexcept;
	void Reset() noexcept;
	void SetPos(DirectX::XMFLOAT3 pos_in) noexcept;
	void Submit(class FrameCommander& frame) const noxnd;
	void Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept;
public:
	struct PointLightData
	{
		alignas(16) DirectX::XMFLOAT3 pos;
		alignas(16) DirectX::XMFLOAT3 ambient;
		alignas(16) DirectX::XMFLOAT3 diffuseColor;
		float diffuseIntensity;
		float attConst;
		float attLin;
		float attQuad;
	};
	struct PointLightCBuf
	{
		long num_point_lights;
		PointLightData pointLights[1000];
		int padding[26];
	};
private:
	PointLightData cbData;
	mutable SolidSphere mesh;
	mutable Bind::PixelConstantBuffer<PointLightCBuf> cbuf;
	static std::vector<PointLightData*> pDataBuffer;
};