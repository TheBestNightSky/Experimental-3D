#include "PointLight.h"
#include "imgui/imgui.h"
#include <vector>
#include <memory>
#include "FrameCommander.h"
#include "DynamicConstant.h"
#include "ConstantBuffersEx.h"

std::shared_ptr<Bind::PixelStructuredBuffer<std::vector<PointLight::PointLightData>>> PointLight::pStructuredBuffer;
std::vector<PointLight::PointLightData*> PointLight::pDataBuffer;
std::unique_ptr<Bind::CachingPixelConstantBufferEx> PointLight::pViewBuffer;
bool PointLight::dirty;

PointLight::PointLight(Graphics& gfx, float radius)
	:
	mesh(gfx, radius)
{
	Reset();
	pDataBuffer.push_back(&(this->cbData));
}

void PointLight::CreateBuffer(Graphics& gfx, DirectX::FXMMATRIX view, std::vector<PointLightData> data) const noexcept
{

	pStructuredBuffer = std::make_shared<Bind::PixelStructuredBuffer<std::vector<PointLightData>>>(gfx, data, 4u);

	Dcb::RawLayout dcbLayout;
	DirectX::XMMATRIX mid = DirectX::XMMatrixTranspose(view);
	DirectX::XMFLOAT4X4 cmView;
	DirectX::XMStoreFloat4x4(&cmView, mid);

	dcbLayout.Add<Dcb::Matrix>("viewMatrix");

	Dcb::Buffer buf{ std::move(dcbLayout) };

	buf["viewMatrix"] = cmView;

	pViewBuffer = std::make_unique<Bind::CachingPixelConstantBufferEx>(gfx, std::move(buf), 2u);
}

void PointLight::SpawnControlWindow(const char* name) noexcept
{
	if (ImGui::Begin(name))
	{
		bool ldirty = false;
		const auto dcheck = [&ldirty](bool changed) {ldirty = ldirty || changed; };
		ImGui::Text("Position");
		dcheck(ImGui::SliderFloat("X", &cbData.pos.x, -60.0f, 60.0f, "%.1f"));
		dcheck(ImGui::SliderFloat("Y", &cbData.pos.y, -60.0f, 60.0f, "%.1f"));
		dcheck(ImGui::SliderFloat("Z", &cbData.pos.z, -60.0f, 60.0f, "%.1f"));

		ImGui::Text("Intensity/Color");
		dcheck(ImGui::SliderFloat("Intensity", &cbData.diffuseIntensity, 0.01f, 2.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp));
		dcheck(ImGui::ColorEdit3("Diffuse Color", &cbData.diffuseColor.x));
		dcheck(ImGui::ColorEdit3("Ambient", &cbData.ambient.x));

		ImGui::Text("Falloff");
		dcheck(ImGui::SliderFloat("Constant", &cbData.attConst, 0.05f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp));
		dcheck(ImGui::SliderFloat("Linear", &cbData.attLin, 0.0001f, 4.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp));
		dcheck(ImGui::SliderFloat("Quadratic", &cbData.attQuad, 0.0000001f, 10.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp));

		if (ImGui::Button("Reset"))
		{
			Reset();
		}

		dirty = dirty || ldirty;
	}
	ImGui::End();
}

void PointLight::Reset() noexcept
{
	cbData = {
		{ 10.0f,9.0f,-2.5f },
		0.0f,
		{ 0.1f,0.1f,0.1f },
		0.0f,
		{ 1.0f,1.0f,1.0f },
		0.0f,
		1.0f,
		1.0f,
		0.0745f,
		0.0366f,
	};
}

void PointLight::SetPos(DirectX::XMFLOAT3 pos_in) noexcept {
	cbData.pos = pos_in;
	dirty = true;
}

void PointLight::SetColor(DirectX::XMFLOAT3 color_in) noexcept {
	cbData.diffuseColor = color_in;
	dirty = true;
}

void PointLight::SetIntensity(float intensity_in) noexcept {
	cbData.diffuseIntensity = intensity_in;
	dirty = true;
}

void PointLight::Submit(FrameCommander& frame) const noxnd
{
	mesh.SetPos(cbData.pos);
	mesh.Submit(frame);
}

void PointLight::Update(Graphics& gfx, std::vector<PointLightData> buf_in) const noexcept{
	pStructuredBuffer.get()->Update(gfx, buf_in);
}

void PointLight::Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept
{
	if (!pDataBuffer.empty()) {

		if (pStructuredBuffer == nullptr || dirty) {
			std::vector<PointLightData> data;

			for (int i = 0; i < pDataBuffer.size(); i++) {
				PointLightData dcopy = *(pDataBuffer[i]);

				data.push_back(dcopy);
			}
			if (pStructuredBuffer == nullptr) {
				CreateBuffer(gfx, view, data);
			}
			Update(gfx, data);
		}

		pStructuredBuffer.get()->Bind(gfx);

		Dcb::RawLayout dcbLayout;
		DirectX::XMMATRIX mid = DirectX::XMMatrixTranspose(view);
		DirectX::XMFLOAT4X4 cmView;
		DirectX::XMStoreFloat4x4(&cmView, mid);

		dcbLayout.Add<Dcb::Matrix>("viewMatrix");

		Dcb::Buffer buf{ std::move(dcbLayout) };

		buf["viewMatrix"] = cmView;

		pViewBuffer->Update(gfx, buf);
		pViewBuffer->Bind(gfx);

	}
}

unsigned int PointLight::GetBufferBindCount() noexcept {
	unsigned int bcount = pStructuredBuffer->GetBindCount();
	pStructuredBuffer->ResetBindCount();
	return bcount;
}