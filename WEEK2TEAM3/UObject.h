#pragma once

#include "Core.h"

struct UObject
{
	FVector Translation;
	FVector Rotation;
	FVector Scale;

	UObject() : Scale(FVector(1.0f, 1.0f, 1.0f)) {}
};