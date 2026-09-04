#pragma once

#include <cmath>

#include "Core.h"

class URenderer
{
	struct FPrimitiveConstants
	{
		FMatrix Matrix;
	};

	struct FViewConstants
	{
		FMatrix View;
		FMatrix Projection;
	};

public:
	// CreateDeviceAndSwapChain
	ID3D11Device* Device = nullptr;
	ID3D11DeviceContext* DeviceContext = nullptr;
	IDXGISwapChain* SwapChain = nullptr;

	// CreateFrameBuffer
	ID3D11Texture2D* FrameBuffer = nullptr;
	ID3D11RenderTargetView* FrameBufferRTV = nullptr;

	ID3D11Texture2D* DeptStencilBuffer = nullptr;
	ID3D11DepthStencilView* DSV = nullptr;

	// CreateRasterizerState
	ID3D11RasterizerState* RasterizerState = nullptr;

	// CreateShader
	ID3D11VertexShader* SimpleVertexShader = nullptr;
	ID3D11PixelShader* SimplePixelShader = nullptr;
	ID3D11InputLayout* SimpleInputLayout = nullptr;

	// CreateConstantBuffer
	ID3D11Buffer* ModelConstantBuffer = nullptr;
	ID3D11Buffer* ViewConstantBuffer = nullptr;

	// values
	UINT Width, Height;
	D3D11_VIEWPORT ViewportInfo;
	FLOAT ClearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f };
	unsigned int Stride;

public:
	void Create(HWND hWindow)
	{
		CreateDeviceAndSwapChain(hWindow);
		CreateFrameBuffer();
		CreateDepthStencilBuffer();
		CreateRasterizerState();
	}

	void Release()
	{
		ReleaseRasterizerState();
		DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
		DSV->Release();
		DeptStencilBuffer->Release();
		ReleaseFrameBuffer();
		ReleaseDeviceAndSwapChain();
	}

	void CreateDeviceAndSwapChain(HWND hWindow)
	{
		D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };

		DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
		swapchaindesc.BufferDesc.Width = 0;
		swapchaindesc.BufferDesc.Height = 0;
		swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapchaindesc.SampleDesc.Count = 1;
		swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchaindesc.BufferCount = 2;
		swapchaindesc.OutputWindow = hWindow;
		swapchaindesc.Windowed = TRUE;
		swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
			nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,
			featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION,
			&swapchaindesc, &SwapChain, &Device, nullptr, &DeviceContext);

		SwapChain->GetDesc(&swapchaindesc);
		Width = swapchaindesc.BufferDesc.Width;
		Height = swapchaindesc.BufferDesc.Height;
		ViewportInfo = { 0.0f, 0.0f, (float)Width, (float)Height, 0.0f, 1.0f };
	}

	void ReleaseDeviceAndSwapChain()
	{
		if (DeviceContext)
		{
			DeviceContext->Flush();
		}

		if (SwapChain)
		{
			SwapChain->Release();
			SwapChain = nullptr;
		}

		if (Device)
		{
			Device->Release();
			Device = nullptr;
		}

		if (DeviceContext)
		{
			DeviceContext->Release();
			DeviceContext = nullptr;
		}
	}

	void CreateFrameBuffer()
	{
		SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&FrameBuffer);

		D3D11_RENDER_TARGET_VIEW_DESC framebufferRTVdesc = {};
		framebufferRTVdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		framebufferRTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		Device->CreateRenderTargetView(FrameBuffer, &framebufferRTVdesc, &FrameBufferRTV);
	}

	void ReleaseFrameBuffer()
	{
		if (FrameBuffer)
		{
			FrameBuffer->Release();
			FrameBuffer = nullptr;
		}

		if (FrameBufferRTV)
		{
			FrameBufferRTV->Release();
			FrameBufferRTV = nullptr;
		}
	}

	void CreateDepthStencilBuffer() 
	{
		D3D11_TEXTURE2D_DESC depthTextureDesc = {};
		depthTextureDesc.Width = Width;
		depthTextureDesc.Height = Height;
		depthTextureDesc.MipLevels = 1;
		depthTextureDesc.ArraySize = 1;
		depthTextureDesc.SampleDesc.Count = 1;
		depthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		Device->CreateTexture2D(&depthTextureDesc, nullptr, &DeptStencilBuffer);

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = depthTextureDesc.Format;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

		Device->CreateDepthStencilView(DeptStencilBuffer, &dsvDesc, &DSV);
	}

	void CreateRasterizerState()
	{
		D3D11_RASTERIZER_DESC rasterizerdesc = {};
		rasterizerdesc.FillMode = D3D11_FILL_SOLID;
		rasterizerdesc.CullMode = D3D11_CULL_NONE;

		Device->CreateRasterizerState(&rasterizerdesc, &RasterizerState);
	}

	void ReleaseRasterizerState()
	{
		if (RasterizerState)
		{
			RasterizerState->Release();
			RasterizerState = nullptr;
		}
	}

	void CreateShader()
	{
		ID3DBlob* vertexshaderCSO;
		ID3DBlob* pixelshaderCSO;

		D3DCompileFromFile(
			L"Assets/Shaders/ShaderW0.hlsl", nullptr, nullptr,
			"mainVS", "vs_5_0", 0, 0, &vertexshaderCSO, nullptr);

		Device->CreateVertexShader(
			vertexshaderCSO->GetBufferPointer(),
			vertexshaderCSO->GetBufferSize(), nullptr, &SimpleVertexShader);

		D3DCompileFromFile(
			L"Assets/Shaders/ShaderW0.hlsl", nullptr, nullptr, "mainPS",
			"ps_5_0", 0, 0, &pixelshaderCSO, nullptr);

		Device->CreatePixelShader(
			pixelshaderCSO->GetBufferPointer(),
			pixelshaderCSO->GetBufferSize(), nullptr, &SimplePixelShader);

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		Device->CreateInputLayout(
			layout, ARRAYSIZE(layout), vertexshaderCSO->GetBufferPointer(),
			vertexshaderCSO->GetBufferSize(), &SimpleInputLayout);

		Stride = sizeof(FVertexSimple);

		vertexshaderCSO->Release();
		pixelshaderCSO->Release();
	}

	void ReleaseShader()
	{
		if (SimpleInputLayout)
		{
			SimpleInputLayout->Release();
			SimpleInputLayout = nullptr;
		}

		if (SimplePixelShader)
		{
			SimplePixelShader->Release();
			SimplePixelShader = nullptr;
		}

		if (SimpleVertexShader)
		{
			SimpleVertexShader->Release();
			SimpleVertexShader = nullptr;
		}
	}

	void CreateConstantBuffer()
	{
		D3D11_BUFFER_DESC constantbufferdesc = {};
		constantbufferdesc.ByteWidth = sizeof(FPrimitiveConstants) + 0xf & 0xfffffff0;
		constantbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
		constantbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		Device->CreateBuffer(&constantbufferdesc, nullptr, &ModelConstantBuffer);

		D3D11_BUFFER_DESC viewconstantbufferdesc = {};
		viewconstantbufferdesc.ByteWidth = sizeof(FViewConstants) + 0xf & 0xfffffff0;
		viewconstantbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
		viewconstantbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		viewconstantbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Device->CreateBuffer(&viewconstantbufferdesc, nullptr, &ViewConstantBuffer);
	}

	void ReleaseConstantBuffer()
	{
		if (ModelConstantBuffer)
		{
			ModelConstantBuffer->Release();
			ModelConstantBuffer = nullptr;
		}
		if (ViewConstantBuffer)
		{
			ViewConstantBuffer->Release();
			ViewConstantBuffer = nullptr;
		}
	}

	void Resize(UINT width, UINT height)
	{
		DeviceContext->OMSetRenderTargets(0, 0, 0);

		FrameBuffer->Release();
		FrameBufferRTV->Release();
		DeptStencilBuffer->Release();
		DSV->Release();

		SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

		Width = width;
		Height = height;
		ViewportInfo = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };

		CreateFrameBuffer();
		CreateDepthStencilBuffer();
	}

	template <typename T>
	ID3D11Buffer* CreateVertexBuffer(T* vertices, UINT byteWidth)
	{
		D3D11_BUFFER_DESC vertexbufferdesc = {};
		vertexbufferdesc.ByteWidth = byteWidth;
		vertexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexbufferSRD = { vertices };

		ID3D11Buffer* vertexBuffer;
		Device->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &vertexBuffer);

		return vertexBuffer;
	}

	void ReleaseVertexBuffer(ID3D11Buffer* vb)
	{
		vb->Release();
	}

	ID3D11Buffer* CreateIndexBuffer(UINT* indices, UINT count)
	{
		D3D11_BUFFER_DESC indexbufferdesc = {};
		indexbufferdesc.ByteWidth = sizeof(UINT) * count;
		indexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexbufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA indexbufferSRD = { indices };

		ID3D11Buffer* indexBuffer;
		Device->CreateBuffer(&indexbufferdesc, &indexbufferSRD, &indexBuffer);
		
		return indexBuffer;
	}

	void ReleaseIndexBuffer(ID3D11Buffer* indexBuffer)
	{
		indexBuffer->Release();
	}

	void Prepare()
	{
		DeviceContext->ClearRenderTargetView(FrameBufferRTV, ClearColor);
		DeviceContext->ClearDepthStencilView(DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		DeviceContext->RSSetViewports(1, &ViewportInfo);
		DeviceContext->RSSetState(RasterizerState);

		DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, DSV);
		DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	}

	void PrepareShader()
	{
		DeviceContext->IASetInputLayout(SimpleInputLayout);

		DeviceContext->VSSetShader(SimpleVertexShader, nullptr, 0);
		if (ModelConstantBuffer)
		{
			DeviceContext->VSSetConstantBuffers(0, 1, &ModelConstantBuffer);
		}
		if (ViewConstantBuffer)
		{
			DeviceContext->VSSetConstantBuffers(1, 1, &ViewConstantBuffer);
		}

		DeviceContext->PSSetShader(SimplePixelShader, nullptr, 0);
	}

	void UpdateModelConstant(FMatrix model)
	{
		if (ModelConstantBuffer)
		{
			D3D11_MAPPED_SUBRESOURCE constantbufferMSR;

			DeviceContext->Map(ModelConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR);
			FPrimitiveConstants* constants = (FPrimitiveConstants*)constantbufferMSR.pData;
			{
				constants->Matrix = model;
			}
			DeviceContext->Unmap(ModelConstantBuffer, 0);
		}
	}

	void UpdateViewConstant(FMatrix view, FMatrix projection)
	{
		if (ViewConstantBuffer)
		{
			D3D11_MAPPED_SUBRESOURCE constantbufferMSR;

			DeviceContext->Map(ViewConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR);
			FViewConstants* constants = (FViewConstants*)constantbufferMSR.pData;
			{
				constants->View = view;
				constants->Projection = projection;
			}
			DeviceContext->Unmap(ViewConstantBuffer, 0);
		}
	}

	void RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices)
	{
		UINT offset = 0;
		DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &offset);
		DeviceContext->Draw(numVertices, 0);
	}

	void SwapBuffer()
	{
		SwapChain->Present(1, 0);
	}

	inline UINT GetWidth() 
	{
		return Width;
	}

	inline UINT GetHeight()
	{
		return Height;
	}
};
