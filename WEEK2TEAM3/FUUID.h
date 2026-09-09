#pragma once

#include "Core.h"

struct FUUID
{
public:
	FUUID();
	FUUID(uint32 _A, uint32 _B, uint32 _C, uint32 _D) : A(_A), B(_B), C(_C), D(_D) {}

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
