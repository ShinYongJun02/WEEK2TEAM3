#pragma once

#include "Core.h"

struct FUUID
{
public:
	FUUID();

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
