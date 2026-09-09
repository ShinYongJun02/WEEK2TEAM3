#include "URenderPipeline.h"
#include "URenderer.h"

void URenderPipeline::Release()
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

	for (int32 i = 0; i < ConstantBuffers.size(); i++)
	{
		if (ConstantBuffers[i])
		{
			ConstantBuffers[i]->Release();
			ConstantBuffers[i] = nullptr;
		}
	}
}

void URenderPipeline::SetRasterRizerState(D3D11_CULL_MODE cullMode, int32 depthBias)
{
	if (RasterizerState)
	{
		RasterizerState->Release();
		RasterizerState = nullptr;
	}

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = cullMode;
	rasterizerDesc.DepthBias = static_cast<INT>(depthBias);
	rasterizerDesc.SlopeScaledDepthBias = depthBias != 0 ? 1.0f : 0.0f;

	Device->CreateRasterizerState(&rasterizerDesc, &RasterizerState);
}

void URenderPipeline::SetDepthStencilState(bool enableDepthTest, bool enableDepthWrite)
{
	if (DepthStencilState)
	{
		DepthStencilState->Release();
		DepthStencilState = nullptr;
	}

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = enableDepthTest;
	depthStencilDesc.DepthWriteMask = enableDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	Device->CreateDepthStencilState(&depthStencilDesc, &DepthStencilState);
}

void URenderPipeline::SetBlendState(const D3D11_BLEND_DESC& blendDesc)
{
	if (BlendState)
	{
		BlendState->Release();
		BlendState = nullptr;
	}

	Device->CreateBlendState(&blendDesc, &BlendState);
}

void URenderPipeline::SetShader(const FString& shaderPath)
{
	int32 size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, shaderPath.c_str(), -1, nullptr, 0);
	if (size == 0)
	{
		return;
	}

	std::wstring wshaderPath(size, L'\0');
	if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, shaderPath.c_str(), -1, wshaderPath.data(), size) == 0)
	{
		return;
	}

	ID3DBlob* vertexshaderCSO;
	ID3DBlob* pixelshaderCSO;

	D3DCompileFromFile(wshaderPath.c_str(), nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &vertexshaderCSO, nullptr);
	Device->CreateVertexShader(vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), nullptr, &VertexShader);

	D3DCompileFromFile(wshaderPath.c_str(), nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &pixelshaderCSO, nullptr);
	Device->CreatePixelShader(pixelshaderCSO->GetBufferPointer(), pixelshaderCSO->GetBufferSize(), nullptr, &PixelShader);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), &InputLayout);
	Stride = sizeof(FVertex);

	vertexshaderCSO->Release();
	pixelshaderCSO->Release();
}
