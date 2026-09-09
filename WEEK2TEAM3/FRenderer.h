#pragma once

#include "Core.h"
#include "FVertex.h"
#include "Helper.h"
#include "FRenderPipeline.h"

class FRenderer
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

	TSharedPtr<FRenderPipeline> DefaultPipeline;
	TSharedPtr<FRenderPipeline> Line2DPipeline;
	TSharedPtr<FRenderPipeline> Circle2DPipeline;
	TSharedPtr<FRenderPipeline> Triangle2DPipeline;
	TSharedPtr<FRenderPipeline> WorldAxisPipeline;
	TSharedPtr<FRenderPipeline> WorldGridPipeline;

	// values
	UINT Width, Height;
	D3D11_VIEWPORT ViewportInfo;
	FMatrix Projection2D;
	FLOAT ClearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f };

public:
	void Create(HWND WindowHandle)
	{
		CreateDeviceAndSwapChain(WindowHandle);
		CreateFrameBuffer();
		CreateDepthStencilBuffer();

		DefaultPipeline = MakeShared<FRenderPipeline>(Device, DeviceContext);
		DefaultPipeline->SetRasterRizerState(D3D11_CULL_BACK);
		DefaultPipeline->SetDepthStencilState(true, true);
		DefaultPipeline->SetShader("Assets/Shaders/ShaderW0.hlsl");
		DefaultPipeline->AddConstantBuffer<FConstants>();
		DefaultPipeline->AddConstantBuffer<FConstants>();

		Line2DPipeline = MakeShared<FRenderPipeline>(Device, DeviceContext);
		Line2DPipeline->SetRasterRizerState(D3D11_CULL_NONE);
		Line2DPipeline->SetDepthStencilState(false, false);
		Line2DPipeline->SetShader("Assets/Shaders/Line2D.hlsl");
		Line2DPipeline->AddConstantBuffer<FLine2DConstants>();

		Circle2DPipeline = MakeShared<FRenderPipeline>(Device, DeviceContext);
		Circle2DPipeline->SetRasterRizerState(D3D11_CULL_NONE);
		Circle2DPipeline->SetDepthStencilState(false, false);
		Circle2DPipeline->SetShader("Assets/Shaders/Circle2D.hlsl");
		Circle2DPipeline->AddConstantBuffer<FCircle2DConstants>();

		Triangle2DPipeline = MakeShared<FRenderPipeline>(Device, DeviceContext);
		Triangle2DPipeline->SetRasterRizerState(D3D11_CULL_NONE);
		Triangle2DPipeline->SetDepthStencilState(false, false);
		Triangle2DPipeline->SetShader("Assets/Shaders/Triangle2D.hlsl");
		Triangle2DPipeline->AddConstantBuffer<FTriangle2DConstants>();

		WorldAxisPipeline = MakeShared<FRenderPipeline>(Device, DeviceContext);
		WorldAxisPipeline->SetRasterRizerState(D3D11_CULL_NONE);
		WorldAxisPipeline->SetDepthStencilState(true, true);
		WorldAxisPipeline->SetShader("Assets/Shaders/WorldAxis.hlsl");
		WorldAxisPipeline->AddConstantBuffer<FWorldAxisConstants>();

		WorldGridPipeline = MakeShared<FRenderPipeline>(Device, DeviceContext);
		WorldGridPipeline->SetRasterRizerState(D3D11_CULL_NONE);
		WorldGridPipeline->SetDepthStencilState(true, false);

		CD3D11_BLEND_DESC BlendDesc = {};
		BlendDesc.RenderTarget[0].BlendEnable = TRUE;
		BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		WorldGridPipeline->SetBlendState(BlendDesc);

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

	void CreateDeviceAndSwapChain(HWND WindowHandle)
	{
		D3D_FEATURE_LEVEL FeatureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

		DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
		SwapChainDesc.BufferDesc.Width = 0;
		SwapChainDesc.BufferDesc.Height = 0;
		SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		SwapChainDesc.SampleDesc.Count = 1;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.BufferCount = 2;
		SwapChainDesc.OutputWindow = WindowHandle;
		SwapChainDesc.Windowed = TRUE;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		UINT CreateDeviceFlags = 0;

		#if defined(_DEBUG)
				CreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		#endif

		D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
			nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT | CreateDeviceFlags,
			FeatureLevels, ARRAYSIZE(FeatureLevels), D3D11_SDK_VERSION,
			&SwapChainDesc, &SwapChain, &Device, nullptr, &DeviceContext);

		SwapChain->GetDesc(&SwapChainDesc);
		Width = SwapChainDesc.BufferDesc.Width;
		Height = SwapChainDesc.BufferDesc.Height;
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

		D3D11_RENDER_TARGET_VIEW_DESC FrameBufferRTVDesc = {};
		FrameBufferRTVDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		FrameBufferRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		Device->CreateRenderTargetView(FrameBuffer, &FrameBufferRTVDesc, &FrameBufferRTV);
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
		D3D11_TEXTURE2D_DESC DepthTextureDesc = {};
		DepthTextureDesc.Width = Width;
		DepthTextureDesc.Height = Height;
		DepthTextureDesc.MipLevels = 1;
		DepthTextureDesc.ArraySize = 1;
		DepthTextureDesc.SampleDesc.Count = 1;
		DepthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DepthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		Device->CreateTexture2D(&DepthTextureDesc, nullptr, &DeptStencilBuffer);

		D3D11_DEPTH_STENCIL_VIEW_DESC DsvDesc = {};
		DsvDesc.Format = DepthTextureDesc.Format;
		DsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

		Device->CreateDepthStencilView(DeptStencilBuffer, &DsvDesc, &DSV);
	}

	void Resize(UINT InWidth, UINT InHeight)
	{
		if (InWidth == 0 || InHeight == 0)
		{
			return;
		}

		DeviceContext->OMSetRenderTargets(0, 0, 0);

		FrameBuffer->Release();
		FrameBufferRTV->Release();
		DeptStencilBuffer->Release();
		DSV->Release();

		SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

		Width = InWidth;
		Height = InHeight;
		ViewportInfo = { 0.0f, 0.0f, (float)InWidth, (float)InHeight, 0.0f, 1.0f };
		Projection2D = Ortho(0.f, Width, Height, 0.f, 0.0f, 1.0f);

		CreateFrameBuffer();
		CreateDepthStencilBuffer();
	}

	template <typename T>
	ID3D11Buffer* CreateVertexBuffer(T* Vertices, UINT ByteWidth)
	{
		D3D11_BUFFER_DESC VertexBufferDesc = {};
		VertexBufferDesc.ByteWidth = ByteWidth;
		VertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA VertexBufferSRD = { Vertices };

		ID3D11Buffer* VertexBuffer;
		Device->CreateBuffer(&VertexBufferDesc, &VertexBufferSRD, &VertexBuffer);

		return VertexBuffer;
	}

	void ReleaseVertexBuffer(ID3D11Buffer* VertexBuffer)
	{
		VertexBuffer->Release();
	}

	ID3D11Buffer* CreateIndexBuffer(UINT* Indices, UINT Count)
	{
		D3D11_BUFFER_DESC IndexBufferDesc = {};
		IndexBufferDesc.ByteWidth = sizeof(UINT) * Count;
		IndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA IndexBufferSRD = { Indices };

		ID3D11Buffer* IndexBuffer;
		Device->CreateBuffer(&IndexBufferDesc, &IndexBufferSRD, &IndexBuffer);

		return IndexBuffer;
	}

	void ReleaseIndexBuffer(ID3D11Buffer* IndexBuffer)
	{
		IndexBuffer->Release();
	}

	TSharedPtr<FRenderPipeline> CreateRenderPipeline()
	{
		return MakeShared<FRenderPipeline>(Device, DeviceContext);
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

	void UpdateModelConstant(FMatrix Model)
	{
		DefaultPipeline->UpdateConstantBuffer(0, FConstants{ Model });
	}

	void UpdateViewConstant(FMatrix View)
	{
		DefaultPipeline->UpdateConstantBuffer(1, FConstants{ View });
	}

	void BindPipeline(const TSharedPtr<FRenderPipeline>& Pipeline) const
	{
		DeviceContext->RSSetState(Pipeline->RasterizerState);
		DeviceContext->OMSetDepthStencilState(Pipeline->DepthStencilState, 0);
		DeviceContext->OMSetBlendState(Pipeline->BlendState, nullptr, 0xffffffff);
		DeviceContext->IASetInputLayout(Pipeline->InputLayout);
		DeviceContext->VSSetShader(Pipeline->VertexShader, nullptr, 0);
		DeviceContext->PSSetShader(Pipeline->PixelShader, nullptr, 0);
		DeviceContext->VSSetConstantBuffers(0, Pipeline->ConstantBuffers.size(), Pipeline->ConstantBuffers.data());
		DeviceContext->PSSetConstantBuffers(0, Pipeline->ConstantBuffers.size(), Pipeline->ConstantBuffers.data());
	}

	void RenderPrimitive(const TSharedPtr<FRenderPipeline>& Pipeline, ID3D11Buffer* Buffer, UINT NumVertices) const
	{
		BindPipeline(Pipeline);

		UINT Offset = 0;
		DeviceContext->IASetVertexBuffers(0, 1, &Buffer, &Pipeline->Stride, &Offset);
		DeviceContext->Draw(NumVertices, 0);
	}

	void RenderPrimitive(ID3D11Buffer* Buffer, UINT NumVertices, const FMatrix& Model) const
	{
		DefaultPipeline->UpdateConstantBuffer(0, FConstants{ Model, FVector4(1.0f, 1.0f, 1.0f, 1.0f), 1 });

		RenderPrimitive(DefaultPipeline, Buffer, NumVertices);
	}

	void RenderPrimitive(ID3D11Buffer* Buffer, UINT NumVertices, const FMatrix& Model, const FVector4& Color) const
	{
		DefaultPipeline->UpdateConstantBuffer(0, FConstants{ Model, Color, 0 });

		RenderPrimitive(DefaultPipeline, Buffer, NumVertices);
	}

	void RenderLine2D(const FVector2& Start, const FVector2& End, const FVector4& Color, float Thickness = 1.0f) const
	{
		Line2DPipeline->UpdateConstantBuffer(0, FLine2DConstants{ Projection2D, Color, Start, End, Thickness });

		BindPipeline(Line2DPipeline);

		UINT Offset = 0;
		DeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, &Offset);
		DeviceContext->Draw(6, 0);
	}

	void RenderCircle2D(const FVector2& Center, const FVector4& Color, float Radius = 1.0f) const
	{
		Circle2DPipeline->UpdateConstantBuffer(0, FCircle2DConstants{ Projection2D, Color, Center, Radius });

		BindPipeline(Circle2DPipeline);

		UINT Offset = 0;
		DeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, &Offset);
		DeviceContext->Draw(6, 0);
	}

	void RenderTriangle2D(const FVector2& Center, const FVector4& Color, float Size = 1.0f, float Rotation = 0.0f) const
	{
		Triangle2DPipeline->UpdateConstantBuffer(0, FTriangle2DConstants{ Projection2D, Color, Center, Size, Rotation - PI * 0.5f });

		BindPipeline(Triangle2DPipeline);

		UINT Offset = 0;
		DeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, &Offset);
		DeviceContext->Draw(3, 0);
	}

	void RenderWorldAxis(const FMatrix& View, const FMatrix& Projection, const FVector4& Color, const FVector& Axis, float Thickness = 1.0f) const
	{
		WorldAxisPipeline->UpdateConstantBuffer(0, FWorldAxisConstants{ View, Projection, Color, Axis, Thickness });

		BindPipeline(WorldAxisPipeline);
		
		UINT Offset = 0;
		DeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, &Offset);
		DeviceContext->Draw(6, 0);
	}

	void RenderWorldGrid(const FMatrix& ViewProjection, const FVector& CameraPosition) const
	{
		WorldGridPipeline->UpdateConstantBuffer(0, FWorldGridConstants{ ViewProjection, CameraPosition });

		BindPipeline(WorldGridPipeline);

		UINT Offset = 0;
		DeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, &Offset);
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
