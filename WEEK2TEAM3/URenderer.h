#pragma once

#include "Core.h"
#include "FVertex.h"
#include "Helper.h"
#include "URenderPipeline.h"

class URenderer
{
	struct FConstants
	{
		FMatrix Matrix;
		FVector4 Color;
		int32 UseVertexColor;
		int32 Padding[3];
	};

	struct FLine2DConstants
	{
		FMatrix Projection;
		FVector4 Color;
		FVector2 Start;
		FVector2 End;
		float Thickness;
		float Padding[3];
	};

	struct FCircle2DConstants
	{
		FMatrix Projection;
		FVector4 Color;
		FVector2 Center;
		float Radius;
		float Padding[2];
	};

	struct FTriangle2DConstants
	{
		FMatrix Projection;
		FVector4 Color;
		FVector2 Center;
		float Size;
		float Rotation;
	};

	struct FWorldAxisConstants
	{
		FMatrix View;
		FMatrix Projection;
		FVector4 Color;
		FVector Axis;
		float Thickness;
	};

	struct FWorldGridConstants
	{
		FMatrix ViewProjection;
		FVector CameraPosition;
		float Padding;
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

	TSharedPtr<URenderPipeline> DefaultPipeline;
	TSharedPtr<URenderPipeline> Line2DPipeline;
	TSharedPtr<URenderPipeline> Circle2DPipeline;
	TSharedPtr<URenderPipeline> Triangle2DPipeline;
	TSharedPtr<URenderPipeline> WorldAxisPipeline;
	TSharedPtr<URenderPipeline> WorldGridPipeline;

	// values
	UINT Width, Height;
	D3D11_VIEWPORT ViewportInfo;
	FMatrix Projection2D;
	FLOAT ClearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f };

public:
	void Create(HWND hWindow)
	{
		CreateDeviceAndSwapChain(hWindow);
		CreateFrameBuffer();
		CreateDepthStencilBuffer();

		DefaultPipeline = MakeShared<URenderPipeline>(Device, DeviceContext);
		DefaultPipeline->SetRasterRizerState(D3D11_CULL_BACK);
		DefaultPipeline->SetDepthStencilState(true, true);
		DefaultPipeline->SetShader("Assets/Shaders/ShaderW0.hlsl");
		DefaultPipeline->AddConstantBuffer<FConstants>();
		DefaultPipeline->AddConstantBuffer<FConstants>();

		Line2DPipeline = MakeShared<URenderPipeline>(Device, DeviceContext);
		Line2DPipeline->SetRasterRizerState(D3D11_CULL_NONE);
		Line2DPipeline->SetDepthStencilState(false, false);
		Line2DPipeline->SetShader("Assets/Shaders/Line2D.hlsl");
		Line2DPipeline->AddConstantBuffer<FLine2DConstants>();

		Circle2DPipeline = MakeShared<URenderPipeline>(Device, DeviceContext);
		Circle2DPipeline->SetRasterRizerState(D3D11_CULL_NONE);
		Circle2DPipeline->SetDepthStencilState(false, false);
		Circle2DPipeline->SetShader("Assets/Shaders/Circle2D.hlsl");
		Circle2DPipeline->AddConstantBuffer<FCircle2DConstants>();

		Triangle2DPipeline = MakeShared<URenderPipeline>(Device, DeviceContext);
		Triangle2DPipeline->SetRasterRizerState(D3D11_CULL_NONE);
		Triangle2DPipeline->SetDepthStencilState(false, false);
		Triangle2DPipeline->SetShader("Assets/Shaders/Triangle2D.hlsl");
		Triangle2DPipeline->AddConstantBuffer<FTriangle2DConstants>();

		WorldAxisPipeline = MakeShared<URenderPipeline>(Device, DeviceContext);
		WorldAxisPipeline->SetRasterRizerState(D3D11_CULL_NONE);
		WorldAxisPipeline->SetDepthStencilState(true, true);
		WorldAxisPipeline->SetShader("Assets/Shaders/WorldAxis.hlsl");
		WorldAxisPipeline->AddConstantBuffer<FWorldAxisConstants>();

		WorldGridPipeline = MakeShared<URenderPipeline>(Device, DeviceContext);
		WorldGridPipeline->SetRasterRizerState(D3D11_CULL_NONE);
		WorldGridPipeline->SetDepthStencilState(true, false);

		CD3D11_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		WorldGridPipeline->SetBlendState(blendDesc);

		WorldGridPipeline->SetShader("Assets/Shaders/WorldGrid.hlsl");
		WorldGridPipeline->AddConstantBuffer<FWorldGridConstants>();
	}

	void Release()
	{
		WorldGridPipeline.reset();
		WorldAxisPipeline.reset();
		Triangle2DPipeline.reset();
		Circle2DPipeline.reset();
		Line2DPipeline.reset();
		DefaultPipeline.reset();
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

		UINT createDeviceFlags = 0;

		#if defined(_DEBUG)
				createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		#endif

		D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
			nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT | createDeviceFlags,
			featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION,
			&swapchaindesc, &SwapChain, &Device, nullptr, &DeviceContext);

		SwapChain->GetDesc(&swapchaindesc);
		Width = swapchaindesc.BufferDesc.Width;
		Height = swapchaindesc.BufferDesc.Height;
		ViewportInfo = { 0.0f, 0.0f, (float)Width, (float)Height, 0.0f, 1.0f };
		Projection2D = Ortho(0.f, Width, Height, 0.f, 0.0f, 1.0f);
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

	void Resize(UINT width, UINT height)
	{
		if (width == 0 || height == 0)
		{
			return;
		}

		DeviceContext->OMSetRenderTargets(0, 0, 0);

		FrameBuffer->Release();
		FrameBufferRTV->Release();
		DeptStencilBuffer->Release();
		DSV->Release();

		SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

		Width = width;
		Height = height;
		ViewportInfo = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
		Projection2D = Ortho(0.f, Width, Height, 0.f, 0.0f, 1.0f);

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

	TSharedPtr<URenderPipeline> CreateRenderPipeline()
	{
		return MakeShared<URenderPipeline>(Device, DeviceContext);
	}

	void Prepare()
	{
		DeviceContext->ClearRenderTargetView(FrameBufferRTV, ClearColor);
		DeviceContext->ClearDepthStencilView(DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		DeviceContext->RSSetViewports(1, &ViewportInfo);

		DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, DSV);
		DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	}

	void UpdateModelConstant(FMatrix model)
	{
		DefaultPipeline->UpdateConstantBuffer(0, FConstants{ model });
	}

	void UpdateViewConstant(FMatrix view)
	{
		DefaultPipeline->UpdateConstantBuffer(1, FConstants{ view });
	}

	void BindPipeline(const TSharedPtr<URenderPipeline>& pipeline) const
	{
		DeviceContext->RSSetState(pipeline->RasterizerState);
		DeviceContext->OMSetDepthStencilState(pipeline->DepthStencilState, 0);
		DeviceContext->OMSetBlendState(pipeline->BlendState, nullptr, 0xffffffff);
		DeviceContext->IASetInputLayout(pipeline->InputLayout);
		DeviceContext->VSSetShader(pipeline->VertexShader, nullptr, 0);
		DeviceContext->PSSetShader(pipeline->PixelShader, nullptr, 0);
		DeviceContext->VSSetConstantBuffers(0, pipeline->ConstantBuffers.size(), pipeline->ConstantBuffers.data());
		DeviceContext->PSSetConstantBuffers(0, pipeline->ConstantBuffers.size(), pipeline->ConstantBuffers.data());
	}

	void RenderPrimitive(const TSharedPtr<URenderPipeline>& pipeline, ID3D11Buffer* pBuffer, UINT numVertices) const
	{
		BindPipeline(pipeline);

		UINT offset = 0;
		DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &pipeline->Stride, &offset);
		DeviceContext->Draw(numVertices, 0);
	}

	void RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices, const FMatrix& model) const
	{
		DefaultPipeline->UpdateConstantBuffer(0, FConstants{ model, FVector4(1.0f, 1.0f, 1.0f, 1.0f), 1 });

		RenderPrimitive(DefaultPipeline, pBuffer, numVertices);
	}

	void RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices, const FMatrix& model, const FVector4& color) const
	{
		DefaultPipeline->UpdateConstantBuffer(0, FConstants{ model, color, 0 });

		RenderPrimitive(DefaultPipeline, pBuffer, numVertices);
	}

	void RenderLine2D(const FVector2& start, const FVector2& end, const FVector4& color, float thickness = 1.0f) const
	{
		Line2DPipeline->UpdateConstantBuffer(0, FLine2DConstants{ Projection2D, color, start, end, thickness });

		BindPipeline(Line2DPipeline);

		UINT offset = 0;
		DeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, &offset);
		DeviceContext->Draw(6, 0);
	}

	void RenderCircle2D(const FVector2& center, const FVector4& color, float radius = 1.0f) const
	{
		Circle2DPipeline->UpdateConstantBuffer(0, FCircle2DConstants{ Projection2D, color, center, radius });

		BindPipeline(Circle2DPipeline);

		UINT offset = 0;
		DeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, &offset);
		DeviceContext->Draw(6, 0);
	}

	void RenderTriangle2D(const FVector2& center, const FVector4& color, float size = 1.0f, float rotation = 0.0f) const
	{
		Triangle2DPipeline->UpdateConstantBuffer(0, FTriangle2DConstants{ Projection2D, color, center, size, rotation - PI * 0.5f });

		BindPipeline(Triangle2DPipeline);

		UINT offset = 0;
		DeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, &offset);
		DeviceContext->Draw(3, 0);
	}

	void RenderWorldAxis(const FMatrix& view, const FMatrix& projection, const FVector4& color, const FVector& axis, float thickness = 1.0f) const
	{
		WorldAxisPipeline->UpdateConstantBuffer(0, FWorldAxisConstants{ view, projection, color, axis, thickness });

		BindPipeline(WorldAxisPipeline);
		
		UINT offset = 0;
		DeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, &offset);
		DeviceContext->Draw(6, 0);
	}

	void RenderWorldGrid(const FMatrix& viewProjection, const FVector& cameraPosition) const
	{
		WorldGridPipeline->UpdateConstantBuffer(0, FWorldGridConstants{ viewProjection, cameraPosition });

		BindPipeline(WorldGridPipeline);

		UINT offset = 0;
		DeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, &offset);
		DeviceContext->Draw(6, 0);
	}

	void SwapBuffer()
	{
		SwapChain->Present(0, 0);
	}

	inline UINT GetWidth() const
	{
		return Width;
	}

	inline UINT GetHeight() const
	{
		return Height;
	}
};
