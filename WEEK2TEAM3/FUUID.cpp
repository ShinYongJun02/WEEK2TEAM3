#include "FUUID.h"

#include <cstdint>
#include <format>
#include <random>

static std::mt19937 GGenerator(std::random_device{}());

FUUID::FUUID()
	: A(0)
	, B(0)
	, C(0)
	, D(0)
{
	std::uniform_int_distribution<uint32> Distribution(0, UINT32_MAX);

	A = Distribution(GGenerator);
	B = Distribution(GGenerator);
	C = Distribution(GGenerator);
	D = Distribution(GGenerator);
}

FUUID::FUUID(uint32 InA, uint32 InB, uint32 InC, uint32 InD)
	: A(InA)
	, B(InB)
	, C(InC)
	, D(InD)
{
}

FString FUUID::ToString() const
{
	return std::format("{}-{}-{}-{}", A, B, C, D);
}

bool FUUID::operator==(const FUUID& Other) const
{
	return A == Other.A && B == Other.B && C == Other.C && D == Other.D;
}

bool FUUID::operator!=(const FUUID& Other) const
{
	return A != Other.A || B != Other.B || C != Other.C || D != Other.D;
}

FUUID FUUID::NewUUID()
{
	std::uniform_int_distribution<uint32> Distribution(0, UINT32_MAX);

	FUUID ResultUUID;
	ResultUUID.A = Distribution(GGenerator);
	ResultUUID.B = Distribution(GGenerator);
	ResultUUID.C = Distribution(GGenerator);
	ResultUUID.D = Distribution(GGenerator);

	return ResultUUID;
}
