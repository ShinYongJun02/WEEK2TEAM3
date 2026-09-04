#pragma once

#include "UPrimitiveComponent.h"
#include "FVertex.h"



class UPlaneComp : public UPrimitiveComponent
{
public:
	UPlaneComp();

	UPlaneComp(URenderer& Renderer);

	~UPlaneComp();

	void setVertexBuffer(URenderer& Renderer);

};