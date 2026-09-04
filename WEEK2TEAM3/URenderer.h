#pragma once

#include <cmath>

#include "Core.h"

class URenderer
{
	struct FConstants
	{
		FMatrix ModelMatrix;
	};

public:
	// CreateDeviceAndSwapChain
	ID3D11Device* Device = nullptr;
	ID3D11DeviceContext* DeviceContext = nullptr;
	IDXGISwapChain* SwapChain = nullptr;

	// CreateFrameBuffer
	ID3D11Texture2D* FrameBuffer = nullptr;
	ID3D11RenderTargetView* FrameBufferRTV = nullptr;

	// CreateRasterizerState
	ID3D11RasterizerState* RasterizerState = nullptr;

	// CreateShader
	ID3D11VertexShader* SimpleVertexShader = nullptr;
	ID3D11PixelShader* SimplePixelShader = nullptr;
	ID3D11InputLayout* SimpleInputLayout = nullptr;

	// CreateConstantBuffer
	ID3D11Buffer* ConstantBuffer = nullptr;

	// values
	D3D11_VIEWPORT ViewportInfo;
	FLOAT ClearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f };
	unsigned int Stride;

public:
	void Create(HWND hWindow)
	{
		CreateDeviceAndSwapChain(hWindow);
		CreateFrameBuffer();
		CreateRasterizerState();
	}

	void Release()
	{
		ReleaseRasterizerState();
		DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
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
		ViewportInfo = { 0.0f, 0.0f,
			(float)swapchaindesc.BufferDesc.Width, (float)swapchaindesc.BufferDesc.Height,
			0.0f, 1.0f };
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

	void CreateRasterizerState()
	{
		D3D11_RASTERIZER_DESC rasterizerdesc = {};
		rasterizerdesc.FillMode = D3D11_FILL_SOLID;
		rasterizerdesc.CullMode = D3D11_CULL_BACK;

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
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
		constantbufferdesc.ByteWidth = sizeof(FConstants) + 0xf & 0xfffffff0;
		constantbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
		constantbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		Device->CreateBuffer(&constantbufferdesc, nullptr, &ConstantBuffer);
	}

	void ReleaseConstantBuffer()
	{
		if (ConstantBuffer)
		{
			ConstantBuffer->Release();
			ConstantBuffer = nullptr;
		}
	}

	ID3D11Buffer* CreateVertexBuffer(FVertexSimple* vertices, UINT byteWidth)
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

	void ReleaseVertexBuffer(ID3D11Buffer* vertexBuffer)
	{
		vertexBuffer->Release();
	}

	void Prepare()
	{
		DeviceContext->ClearRenderTargetView(FrameBufferRTV, ClearColor);

		DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		DeviceContext->RSSetViewports(1, &ViewportInfo);
		DeviceContext->RSSetState(RasterizerState);

		DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, nullptr);
		DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	}

	void PrepareShader()
	{
		DeviceContext->IASetInputLayout(SimpleInputLayout);

		DeviceContext->VSSetShader(SimpleVertexShader, nullptr, 0);
		if (ConstantBuffer)
		{
			DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
		}

		DeviceContext->PSSetShader(SimplePixelShader, nullptr, 0);
	}

	void UpdateConstant(FVector Translation, FVector Rotation, FVector Scale)
	{
		if (ConstantBuffer)
		{
			D3D11_MAPPED_SUBRESOURCE constantbufferMSR;

			DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR);
			FConstants* constants = (FConstants*)constantbufferMSR.pData;
			{
				FMatrix S(
					FVector4(Scale.x, 0.0f, 0.0f, 0.0f),
					FVector4(0.0f, Scale.y, 0.0f, 0.0f),
					FVector4(0.0f, 0.0f, Scale.z, 0.0f),
					FVector4(0.0f, 0.0f, 0.0f, 1.0f));

				float radianX = Rotation.x * acos(-1) / 180.0f;
				FMatrix RX(
					FVector4(1.0f, 0.0f, 0.0f, 0.0f),
					FVector4(0.0f, cos(radianX), sin(radianX), 0.0f),
					FVector4(0.0f, -sin(radianX), cos(radianX), 0.0f),
					FVector4(0.0f, 0.0f, 0.0f, 1.0f));

				float radianY = Rotation.y * acos(-1) / 180.0f;
				FMatrix RY(
					FVector4(cos(radianY), 0.0f, -sin(radianY), 0.0f),
					FVector4(0.0f, 1.0f, 0.0f, 0.0f),
					FVector4(sin(radianY), 0.0f, cos(radianY), 0.0f),
					FVector4(0.0f, 0.0f, 0.0f, 1.0f));

				float radianZ = Rotation.z * acos(-1) / 180.0f;
				FMatrix RZ(
					FVector4(cos(radianZ), sin(radianZ), 0.0f, 0.0f),
					FVector4(-sin(radianZ), cos(radianZ), 0.0f, 0.0f),
					FVector4(0.0f, 0.0f, 1.0f, 0.0f),
					FVector4(0.0f, 0.0f, 0.0f, 1.0f));

				FMatrix T(
					FVector4(1.0f, 0.0f, 0.0f, 0.0f),
					FVector4(0.0f, 1.0f, 0.0f, 0.0f),
					FVector4(0.0f, 0.0f, 1.0f, 0.0f),
					FVector4(Translation.x, Translation.y, Translation.z, 1.0f));

				// 모델 매트릭스 생성 M = S * R * T
				constants->ModelMatrix = S * RX * RY * RZ * T;
			}
			DeviceContext->Unmap(ConstantBuffer, 0);
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
};
