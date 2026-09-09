#pragma once

#include <d3d11.h>

#include "UObject.h"

class FRenderer;

struct FStaticMesh
{
	ID3D11Buffer* VertexBuffer;
	UINT VertexCount;
	FVector MinVertex;
	FVector MaxVertex;
};

class UResourceManager : public UObject
{
public:
	void Initialize(FRenderer& InRenderer);
	void Release();

	TSharedPtr<FStaticMesh> GetStaticMesh(const FString& NameKey);

private:
	void BuiltInStaticMeshes();

private:
	FRenderer* Renderer;

	TMap<FString, TSharedPtr<FStaticMesh>> StaticMeshMap;
};
