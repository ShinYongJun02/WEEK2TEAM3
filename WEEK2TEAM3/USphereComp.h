#pragma once
#include "UPrimitiveComponent.h"
#include "FVertex.h"



class USphereComp : public UPrimitiveComponent
{
	//ID3D11Buffer* vertexBufferCube;

	USphereComp();

	USphereComp(URenderer& Renderer);
	
	~USphereComp();

	void setVertexBuffer(URenderer& Renderer);
};