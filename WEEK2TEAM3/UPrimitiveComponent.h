#pragma once
#include "USceneComponent.h"
#include "URenderer.h"


class UPrimitiveComponent : public USceneComponent
{
public:
	ID3D11Buffer* VertexBuffer = nullptr;
	uint32 NumVertices = 0;

	virtual void Render(URenderer& Renderer);

	virtual ~UPrimitiveComponent();
};
