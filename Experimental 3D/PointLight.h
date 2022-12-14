#pragma once
#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffers.h"
#include "StructuredBuffers.h"
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
	void SetColor(DirectX::XMFLOAT3 color_in) noexcept;
	void SetIntensity(float intensity_in) noexcept;
	void CreateBuffer(Graphics& gfx) const noexcept;
	void Submit(class FrameCommander& frame) const noxnd;
	void Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept;
public:
	struct PointLightData
	{
		DirectX::XMFLOAT3 pos;
		float pad0;
		DirectX::XMFLOAT3 ambient;
		float pad1;
		DirectX::XMFLOAT3 diffuseColor;
		float pad2;
		float diffuseIntensity;
		float attConst;
		float attLin;
		float attQuad;
	};
private:
	void Update(Graphics& gfx, std::vector<PointLightData> buf_in) const noexcept;
private:
	PointLightData cbData;
	mutable SolidSphere mesh;
	static std::shared_ptr<Bind::PixelStructuredBuffer<std::vector<PointLightData>>> pStructuredBuffer;
	static std::vector<PointLightData*> pDataBuffer;
	static bool dirty;
};