#pragma once

#include "Core.h"

class URenderer;

class URenderPipeline
{
public:
	URenderPipeline(ID3D11Device* device, ID3D11DeviceContext* deviceContext) 
		: Device(device)
		, DeviceContext(deviceContext) 
	{
	}

	~URenderPipeline() 
	{
		Release();
	}

	void Release();

	void SetCullMode(D3D11_CULL_MODE cullMode);
	void SetDepthStencilState(bool enableDepthTest, bool enableDepthWrite);
	void SetShader(const FString& shaderPath);

	template <typename T>
	void AddConstantBuffer()
	{
		if (Device)
		{
			D3D11_BUFFER_DESC constantBufferDesc = {};
			constantBufferDesc.ByteWidth = sizeof(T) + 0xf & 0xfffffff0;
			constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			ID3D11Buffer* constantBuffer = nullptr;
			HRESULT hr = Device->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer);
			if (SUCCEEDED(hr))
			{
				ConstantBuffers.push_back(constantBuffer);
			}
		}
	}

	template <typename T>
	void UpdateConstantBuffer(uint32 index, const T& data)
	{
		if (DeviceContext && index < ConstantBuffers.size())
		{
			ID3D11Buffer* constantBuffer = ConstantBuffers[index];
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			HRESULT hr = DeviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if (SUCCEEDED(hr))
			{
				memcpy(mappedResource.pData, &data, sizeof(T));
				DeviceContext->Unmap(constantBuffer, 0);
			}
		}
	}

private:
	friend class URenderer;

	ID3D11Device* Device = nullptr;
	ID3D11DeviceContext* DeviceContext = nullptr;

	ID3D11RasterizerState* RasterizerState = nullptr;
	ID3D11DepthStencilState* DepthStencilState = nullptr;
	ID3D11InputLayout* InputLayout = nullptr;
	uint32 Stride = 0;
	ID3D11VertexShader* VertexShader = nullptr;
	ID3D11PixelShader* PixelShader = nullptr;
	TArray<ID3D11Buffer*> ConstantBuffers;
};