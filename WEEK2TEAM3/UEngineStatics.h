#pragma once

#include "Core.h"

struct FUUID
{
	uint32 A;
	uint32 B;
	uint32 C;
	uint32 D;

	bool operator==(const FUUID& Other) const
	{
		return A == Other.A && B == Other.B && C == Other.C && D == Other.D;
	}

	bool operator!=(const FUUID& Other) const
	{
		return A != Other.A || B != Other.B || C != Other.C || D != Other.D;
	}
};

class UEngineStatics
{
public:
	static FUUID GetUUID();
};