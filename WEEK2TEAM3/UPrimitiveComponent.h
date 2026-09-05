#pragma once
#include "USceneComponent.h"
#include "URenderer.h"
#include <wrl.h>


class UPrimitiveComponent : public USceneComponent
{
public:
	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer = nullptr;
	uint32 NumVertices = 0;

	virtual void Render(URenderer& Renderer);

	virtual ~UPrimitiveComponent();
};
