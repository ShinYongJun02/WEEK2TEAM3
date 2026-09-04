#pragma once
#include "UPrimitiveComponent.h"
#include "FVertex.h"



class UCubeComp : public UPrimitiveComponent
{
public:
	//ID3D11Buffer* vertexBufferCube;

	UCubeComp();

	UCubeComp(URenderer& Renderer);

	~UCubeComp();
	
	void setVertexBuffer(URenderer& Renderer);

};