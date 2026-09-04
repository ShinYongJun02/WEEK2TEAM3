#pragma once
#include "UPrimitiveComponent.h"
#include "FVertex.h"



class UCubeComp : public UPrimitiveComponent
{
public:
	UCubeComp();

	UCubeComp(URenderer& Renderer);

	~UCubeComp();
	
	void setVertexBuffer(URenderer& Renderer);

};