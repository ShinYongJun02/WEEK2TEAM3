#pragma once

#include <windows.h>
#include <d3d11.h>

#include "Core.h"
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
	void Create(HWND WindowHandle);
	void Release();

	void CreateDeviceAndSwapChain(HWND WindowHandle);
	void ReleaseDeviceAndSwapChain();

	void CreateFrameBuffer();
	void ReleaseFrameBuffer();

	void CreateDepthStencilBuffer();

	void Resize(UINT InWidth, UINT InHeight);

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

	void ReleaseVertexBuffer(ID3D11Buffer* VertexBuffer);

	ID3D11Buffer* CreateIndexBuffer(UINT* Indices, UINT Count);
	void ReleaseIndexBuffer(ID3D11Buffer* IndexBuffer);

	TSharedPtr<FRenderPipeline> CreateRenderPipeline();

	void Prepare();

	void UpdateModelConstant(FMatrix Model);
	void UpdateViewConstant(FMatrix View);

	void BindPipeline(const TSharedPtr<FRenderPipeline>& Pipeline) const;

	void RenderPrimitive(const TSharedPtr<FRenderPipeline>& Pipeline, ID3D11Buffer* Buffer, UINT NumVertices) const;
	void RenderPrimitive(ID3D11Buffer* Buffer, UINT NumVertices, const FMatrix& Model) const;
	void RenderPrimitive(ID3D11Buffer* Buffer, UINT NumVertices, const FMatrix& Model, const FVector4& Color) const;

	void RenderLine2D(const FVector2& Start, const FVector2& End, const FVector4& Color, float Thickness = 1.0f) const;
	void RenderCircle2D(const FVector2& Center, const FVector4& Color, float Radius = 1.0f) const;
	void RenderTriangle2D(const FVector2& Center, const FVector4& Color, float Size = 1.0f, float Rotation = 0.0f) const;
	void RenderWorldAxis(const FMatrix& View, const FMatrix& Projection, const FVector4& Color, const FVector& Axis, float Thickness = 1.0f) const;
	void RenderWorldGrid(const FMatrix& ViewProjection, const FVector& CameraPosition) const;

	void SwapBuffer();

	UINT GetWidth() const;
	UINT GetHeight() const;
};
