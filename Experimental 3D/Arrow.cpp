#include "Arrow.h"
#include "BindableBase.h"
#include "GraphicsExceptionMacros.h"
#include "SkyeFun.h"

Arrow::Arrow(Graphics& gfx, float x, float y, float rad, float r, float g, float b)
	:
	X(x),
	Y(y),
	Rad(rad),
	R(r),
	G(g),
	B(b) {

	WndInfo wnd = sf::GetWndInfo(*gfx.GetHWND());
	width = wnd.width;
	height = wnd.height;

	if (!IsStaticInitialized()) {

		struct Vertex {
			float x;
			float y;
			float r;
			float g;
			float b;
		};

		const std::vector<Vertex> vertices = {
			{ -0.25f, 0.25f , r, g, b},
			{ 0.25f, 0.25f , r, g, b},
			{ 0.0f, 0.0f , r, g, b},

			{-0.125f, 0.5f, r, g, b},
			{0.0f, 0.5f, r, g, b},
			{-0.0625f, 0.25f, r, g, b},

			{0.0625f, 0.25f, r, g, b},
			{0.125f, 0.5f, r, g, b},
		};
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));

		auto pvs = std::make_unique<VertexShader>(gfx, L"VertexShader.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PixelShader.cso"));

		const std::vector<unsigned short> indices = {
			0, 1, 2,
			3, 4, 5,
			4, 7, 6,
			4, 6, 5,
		};
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else {

		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}

void Arrow::Update(float dt) noexcept{

}

void Arrow::SetPos(float x, float y, float rad) noexcept {
	X = (x / (width / 2.0f)) - 1.0f;
	Y = (-y / (height / 2.0f)) + 1.0f;
	Rad = rad;
}

void Arrow::SetColor(float r, float g, float b) noexcept {
	R = r;
	G = g;
	B = b;
}

DirectX::XMMATRIX Arrow::GetTransformXM() const noexcept {
	return DirectX::XMMatrixRotationZ(Rad) * 
		DirectX::XMMatrixTranslation(X, Y, 0.0f) *
		DirectX::XMMatrixScaling(height / width, 1.0f, 1.0f);
		
}