#include "UPrimitiveComponent.h"

void UPrimitiveComponent::Render(URenderer& Renderer)
{
	if (!StaticMesh || StaticMesh->VertexBuffer == nullptr)
	{
		return;
	}

	FMatrix model = GetModelMatrix();
	Renderer.UpdateModelConstant(model);
	Renderer.RenderPrimitive(StaticMesh->VertexBuffer, StaticMesh->VertexCount);
}

// Möller–Trumbore 방식
bool UPrimitiveComponent::IntersectLocal(const FRay& localRay, float& outT) const
{
	const FVertex* Vertexes = StaticMesh->Vertices;
	uint32 NumVertexes = StaticMesh->VertexCount;

	FVector O = localRay.Origin;
	FVector D = localRay.Direction;

	float MinT = FLT_MAX;

	for (uint32 i = 0; i + 2 < NumVertexes; i += 3)
	{
		FVector V0(Vertexes[i].x, Vertexes[i].y, Vertexes[i].z);
		FVector V1(Vertexes[i + 1].x, Vertexes[i + 1].y, Vertexes[i + 1].z);
		FVector V2(Vertexes[i + 2].x, Vertexes[i + 2].y, Vertexes[i + 2].z);

		FVector E1 = V1 - V0;
		FVector E2 = V2 - V0;

		FVector P = D.Cross(E2);

		float det = P.Dot(E1);

		if (det < 1e-8f)
		{
			continue;
		}

		FVector T = O - V0;
		float InvDet = 1.0f / det;

		float u = (P.Dot(T)) * InvDet;

		if (u < 0.0f || u>1.0f)
		{
			continue;
		}

		FVector Q = T.Cross(E1);
		float t = (Q.Dot(E2)) * InvDet;

		if (t <= 0.0f)
		{
			continue;
		}

		float v = (Q.Dot(D)) * InvDet;

		if (v < 0.0f || u + v>1.0f)
		{
			continue;
		}

		MinT = min(MinT, t);
	}

	if (MinT == FLT_MAX)
	{
		return false;
	}

	outT = MinT;
	return true;
}
