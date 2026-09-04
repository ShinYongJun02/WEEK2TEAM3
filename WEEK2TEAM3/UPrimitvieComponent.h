#pragma once

#include "USceneComponent.h"

class URenderer;

class UPrimitiveComponent : public USceneComponent
{
public:
	UPrimitiveComponent(URenderer& renderer);

	void Render();

protected:
	URenderer& Renderer;

	ID3D11Buffer* VertexBuffer;
	uint32 VertexCount;
};

class UCubeComp : public UPrimitiveComponent
{
public:
	UCubeComp(URenderer& renderer);
};

class USphereComp : public UPrimitiveComponent
{
public:
	USphereComp(URenderer& renderer);
};

class UPlaneComp : public UPrimitiveComponent
{
public:
	UPlaneComp(URenderer& renderer);
};