#pragma once

#include "UObject.h"

class URenderer;

struct FStaticMesh
{
	ID3D11Buffer* VertexBuffer;
	UINT VertexCount;
};

class UResourceManager : public UObject
{
public:
	void Initialize(URenderer& renderer);
	void Release();

	TSharedPtr<FStaticMesh> GetStaticMesh(const FString& NameKey);

private:
	void BuiltInStaticMeshes();

private:
	URenderer* Renderer;

	TMap<FString, TSharedPtr<FStaticMesh>> StaticMeshMap;
};