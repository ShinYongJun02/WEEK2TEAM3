#pragma once
#include "UPrimitiveComponent.h"
#include "FVertex.h"



class USphereComp : public UPrimitiveComponent
{
public:
	USphereComp();

	USphereComp(URenderer& Renderer);
	
	~USphereComp();

	void setVertexBuffer(URenderer& Renderer);
};