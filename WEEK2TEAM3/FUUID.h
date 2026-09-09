#pragma once

#include "Core.h"

struct FUUID
{
public:
	FUUID();
	FUUID(uint32 InA, uint32 InB, uint32 InC, uint32 InD) : A(InA), B(InB), C(InC), D(InD) {}

	FString ToString() const;

	bool operator==(const FUUID& Other) const
	{
		return A == Other.A && B == Other.B && C == Other.C && D == Other.D;
	}

	bool operator!=(const FUUID& Other) const
	{
		return A != Other.A || B != Other.B || C != Other.C || D != Other.D;
	}

	static FUUID NewUUID();

private:
	uint32 A;
	uint32 B;
	uint32 C;
	uint32 D;
};
