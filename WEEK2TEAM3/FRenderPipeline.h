#pragma once

#include <d3d11.h>

#include "Core.h"

class FRenderer;

class FRenderPipeline
{
public:
	FRenderPipeline(ID3D11Device* InDevice, ID3D11DeviceContext* InDeviceContext);
	~FRenderPipeline();

	void Release();

	void SetRasterRizerState(D3D11_CULL_MODE CullMode, int32 DepthBias = 0);
	void SetDepthStencilState(bool bEnableDepthTest, bool bEnableDepthWrite);
	void SetBlendState(const D3D11_BLEND_DESC& BlendDesc);
	void SetShader(const FString& ShaderPath);

	template <typename T>
	void AddConstantBuffer()
	{
		if (Device)
		{
			D3D11_BUFFER_DESC ConstantBufferDesc = {};
			ConstantBufferDesc.ByteWidth = sizeof(T) + 0xf & 0xfffffff0;
			ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			ID3D11Buffer* ConstantBuffer = nullptr;
			HRESULT Hr = Device->CreateBuffer(&ConstantBufferDesc, nullptr, &ConstantBuffer);
			if (SUCCEEDED(Hr))
			{
				ConstantBuffers.push_back(ConstantBuffer);
			}
		}
	}

	template <typename T>
	void UpdateConstantBuffer(uint32 Index, const T& Data)
	{
		if (DeviceContext && Index < ConstantBuffers.size())
		{
			ID3D11Buffer* ConstantBuffer = ConstantBuffers[Index];
			D3D11_MAPPED_SUBRESOURCE MappedResource;
			HRESULT Hr = DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
			if (SUCCEEDED(Hr))
			{
				memcpy(MappedResource.pData, &Data, sizeof(T));
				DeviceContext->Unmap(ConstantBuffer, 0);
			}
		}
	}

private:
	friend class FRenderer;

	ID3D11Device* Device = nullptr;
	ID3D11DeviceContext* DeviceContext = nullptr;

	ID3D11RasterizerState* RasterizerState = nullptr;
	ID3D11DepthStencilState* DepthStencilState = nullptr;
	ID3D11InputLayout* InputLayout = nullptr;
	ID3D11BlendState* BlendState = nullptr;
	uint32 Stride = 0;
	ID3D11VertexShader* VertexShader = nullptr;
	ID3D11PixelShader* PixelShader = nullptr;
	TArray<ID3D11Buffer*> ConstantBuffers;
};
