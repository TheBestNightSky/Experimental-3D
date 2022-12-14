#pragma once
#include "Bindable.h"
#include "GraphicsExceptionMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	template<typename C>
	class StructuredBuffer : public Bindable
	{
	public:
		void Update(Graphics& gfx, const C& consts)
		{
			INFOMAN(gfx);

			size_t sizeInBytes = consts.size() * sizeof(consts[0]);

			D3D11_MAPPED_SUBRESOURCE msr;
			GFX_THROW_INFO(GetContext(gfx)->Map(
				pConstantBuffer.Get(), 0u,
				D3D11_MAP_WRITE_DISCARD, 0u,
				&msr
			));
			memcpy_s(msr.pData,sizeInBytes, consts.data(), sizeInBytes);
			GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u);
		}
		StructuredBuffer(Graphics& gfx, const C& consts, UINT slot = 0u)
			:
			slot(slot)
		{
			INFOMAN(gfx);

			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			cbd.ByteWidth = sizeof(consts[0]) * consts.size();
			cbd.StructureByteStride = sizeof(consts[0]);

			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem = consts.data();
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &pConstantBuffer));

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srvDesc.Buffer.FirstElement = 0;
			srvDesc.Buffer.NumElements = consts.size();

			GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pConstantBuffer.Get(), &srvDesc, &pSrv));
		}
		StructuredBuffer(Graphics& gfx, UINT slot = 0u)
			:
			slot(slot)
		{
			INFOMAN(gfx);

			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(C);
			cbd.StructureByteStride = 0u;
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
		}
	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSrv;
		UINT slot;
	};

	template<typename C>
	class PixelStructuredBuffer : public StructuredBuffer<C>
	{
		using StructuredBuffer<C>::pSrv;
		using StructuredBuffer<C>::pConstantBuffer;
		using StructuredBuffer<C>::slot;
		using Bindable::GetContext;
	public:
		using StructuredBuffer<C>::StructuredBuffer;
		void Bind(Graphics& gfx) noexcept override
		{
			GetContext(gfx)->PSSetShaderResources(slot, 1u, pSrv.GetAddressOf());
		}
		static std::shared_ptr<PixelStructuredBuffer> Resolve(Graphics& gfx, const C& consts, UINT slot = 0)
		{
			return Codex::Resolve<PixelStructuredBuffer>(gfx, consts, slot);
		}
		static std::shared_ptr<PixelStructuredBuffer> Resolve(Graphics& gfx, UINT slot = 0)
		{
			return Codex::Resolve<PixelStructuredBuffer>(gfx, slot);
		}
		static std::string GenerateUID(const C&, UINT slot)
		{
			return GenerateUID(slot);
		}
		static std::string GenerateUID(UINT slot = 0)
		{
			using namespace std::string_literals;
			return typeid(PixelStructuredBuffer).name() + "#"s + std::to_string(slot);
		}
		std::string GetUID() const noexcept override
		{
			return GenerateUID(slot);
		}
	};
}