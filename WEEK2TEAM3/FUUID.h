#pragma once

#include "Core.h"

struct FUUID
{
public:
	FUUID();
	FUUID(uint32 InA, uint32 InB, uint32 InC, uint32 InD);

	FString ToString() const;

	bool operator==(const FUUID& Other) const;
	bool operator!=(const FUUID& Other) const;

	static FUUID NewUUID();

private:
	uint32 A;
	uint32 B;
	uint32 C;
	uint32 D;
};
