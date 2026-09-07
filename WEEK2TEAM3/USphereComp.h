#pragma once

#include "UPrimitiveComponent.h"
#include "FVertex.h"

class USphereComp : public UPrimitiveComponent
{
	// scale 1.0 기준
	float radius = 0.5f;

public:
	DECLARE_CLASS(USphereComp, UPrimitiveComponent)

	//bool IntersectLocal(const FRay& localRay, float& outT) const
	//{
	//	// 구 방정식: |X| = R
	//	// 광선 위의 점 방정식: Origin + Direction * t
	//	// 구 위의 점 && 광선 위의 점 = (O + D * t)^2 = R^2
	//	// a * t^2 + b * t + c = 0
	//	const FVector& O = localRay.Origin;
	//	const FVector& D = localRay.Direction;

	//	float a = D.Dot(D);
	//	float halfb = O.Dot(D);
	//	float c = O.Dot(O) - radius * radius;

	//	// 판정식
	//	float d = halfb * halfb - a * c;
	//	if (d < 0.0f) return false;

	//	// 거리
	//	float t = (-halfb - sqrt(d)) / a;

	//	// 구가 광원 뒤에 있음
	//	if (t < 0.0f) return false;

	//	outT = t;
	//	return true;
	//}
};