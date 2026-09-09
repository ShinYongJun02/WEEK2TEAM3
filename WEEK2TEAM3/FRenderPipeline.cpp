#include "FRenderPipeline.h"

#include <windows.h>
#include <d3dcompiler.h>

#include "FVertex.h"

FRenderPipeline::FRenderPipeline(ID3D11Device* InDevice, ID3D11DeviceContext* InDeviceContext)
	: Device(InDevice)
	, DeviceContext(InDeviceContext)
{
}

FRenderPipeline::~FRenderPipeline()
{
	Release();
}

void FRenderPipeline::Release()
{
	if (RasterizerState)
	{
		RasterizerState->Release();
		RasterizerState = nullptr;
	}

	if (DepthStencilState)
	{
		DepthStencilState->Release();
		DepthStencilState = nullptr;
	}

	if (BlendState)
	{
		BlendState->Release();
		BlendState = nullptr;
	}

	if (VertexShader)
	{
		VertexShader->Release();
		VertexShader = nullptr;
	}

	if (PixelShader)
	{
		PixelShader->Release();
		PixelShader = nullptr;
	}

	if (InputLayout)
	{
		InputLayout->Release();
		InputLayout = nullptr;
	}

	for (int32 Index = 0; Index < ConstantBuffers.size(); Index++)
	{
		if (ConstantBuffers[Index])
		{
			ConstantBuffers[Index]->Release();
			ConstantBuffers[Index] = nullptr;
		}
	}
}

void FRenderPipeline::SetRasterRizerState(D3D11_CULL_MODE CullMode, int32 DepthBias)
{
	if (RasterizerState)
	{
		RasterizerState->Release();
		RasterizerState = nullptr;
	}

	D3D11_RASTERIZER_DESC RasterizerDesc = {};
	RasterizerDesc.FillMode = D3D11_FILL_SOLID;
	RasterizerDesc.CullMode = CullMode;
	RasterizerDesc.DepthBias = static_cast<INT>(DepthBias);
	RasterizerDesc.SlopeScaledDepthBias = DepthBias != 0 ? 1.0f : 0.0f;

	Device->CreateRasterizerState(&RasterizerDesc, &RasterizerState);
}

void FRenderPipeline::SetDepthStencilState(bool bEnableDepthTest, bool bEnableDepthWrite)
{
	if (DepthStencilState)
	{
		DepthStencilState->Release();
		DepthStencilState = nullptr;
	}

	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc = {};
	DepthStencilDesc.DepthEnable = bEnableDepthTest;
	DepthStencilDesc.DepthWriteMask = bEnableDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState);
}

void FRenderPipeline::SetBlendState(const D3D11_BLEND_DESC& BlendDesc)
{
	if (BlendState)
	{
		BlendState->Release();
		BlendState = nullptr;
	}

	Device->CreateBlendState(&BlendDesc, &BlendState);
}

void FRenderPipeline::SetShader(const FString& ShaderPath)
{
	int32 Size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, ShaderPath.c_str(), -1, nullptr, 0);
	if (Size == 0)
	{
		return;
	}

	std::wstring WShaderPath(Size, L'\0');
	if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, ShaderPath.c_str(), -1, WShaderPath.data(), Size) == 0)
	{
		return;
	}

	ID3DBlob* VertexShaderCSO;
	ID3DBlob* PixelShaderCSO;

	D3DCompileFromFile(WShaderPath.c_str(), nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &VertexShaderCSO, nullptr);
	Device->CreateVertexShader(VertexShaderCSO->GetBufferPointer(), VertexShaderCSO->GetBufferSize(), nullptr, &VertexShader);

	D3DCompileFromFile(WShaderPath.c_str(), nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &PixelShaderCSO, nullptr);
	Device->CreatePixelShader(PixelShaderCSO->GetBufferPointer(), PixelShaderCSO->GetBufferSize(), nullptr, &PixelShader);

	D3D11_INPUT_ELEMENT_DESC Layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	Device->CreateInputLayout(Layout, ARRAYSIZE(Layout), VertexShaderCSO->GetBufferPointer(), VertexShaderCSO->GetBufferSize(), &InputLayout);
	Stride = sizeof(FVertex);

	VertexShaderCSO->Release();
	PixelShaderCSO->Release();
}
