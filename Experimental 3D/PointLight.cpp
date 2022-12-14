#include "PointLight.h"
#include "imgui/imgui.h"
#include <vector>
#include <memory>
#include "FrameCommander.h"

PointLight::PointLight(Graphics& gfx, float radius)
	:
	mesh(gfx, radius)
{
	Reset();
	pDataBuffer.push_back(&(this->cbData));
}

void PointLight::CreateBuffer(Graphics& gfx) const noexcept
{
	std::vector<PointLightData> data;

	for (int i = 0; i < pDataBuffer.size(); i++) {
		data.push_back(*(pDataBuffer[i]));
	}

	pStructuredBuffer = Bind::PixelStructuredBuffer<std::vector<PointLightData>>::Resolve(gfx, data, 4u);
}

void PointLight::SpawnControlWindow(const char* name) noexcept
{
	if (ImGui::Begin(name))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &cbData.pos.x, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Y", &cbData.pos.y, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Z", &cbData.pos.z, -60.0f, 60.0f, "%.1f");

		ImGui::Text("Intensity/Color");
		ImGui::SliderFloat("Intensity", &cbData.diffuseIntensity, 0.01f, 2.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::ColorEdit3("Diffuse Color", &cbData.diffuseColor.x);
		ImGui::ColorEdit3("Ambient", &cbData.ambient.x);

		ImGui::Text("Falloff");
		ImGui::SliderFloat("Constant", &cbData.attConst, 0.05f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::SliderFloat("Linear", &cbData.attLin, 0.0001f, 4.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::SliderFloat("Quadratic", &cbData.attQuad, 0.0000001f, 10.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);

		if (ImGui::Button("Reset"))
		{
			Reset();
		}
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
		if (pStructuredBuffer == nullptr) {
			CreateBuffer(gfx);
		}

		if (dirty) {
			std::vector<PointLightData> data;

			for (int i = 0; i < pDataBuffer.size(); i++) {
				data.push_back(*(pDataBuffer[i]));

				const auto pos = DirectX::XMLoadFloat3(&data[i].pos);
				DirectX::XMStoreFloat3(&data[i].pos, DirectX::XMVector3Transform(pos, view));
			}

			Update(gfx, data);
			dirty = false;
		}
		pStructuredBuffer.get()->Bind(gfx);

	}
}

std::shared_ptr<Bind::PixelStructuredBuffer<std::vector<PointLight::PointLightData>>> PointLight::pStructuredBuffer;
std::vector<PointLight::PointLightData*> PointLight::pDataBuffer;
bool PointLight::dirty;