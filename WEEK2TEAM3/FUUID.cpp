#include "FUUID.h"
#include <random>
#include <format>

static std::mt19937 GGenerator(std::random_device{}());

FUUID::FUUID()
	: A(0)
	, B(0)
	, C(0)
	, D(0)
{
	std::uniform_int_distribution<uint32> distribution(0, UINT32_MAX);

	A = distribution(GGenerator);
	B = distribution(GGenerator);
	C = distribution(GGenerator);
	D = distribution(GGenerator);
}

FString FUUID::ToString() const
{
	return std::format("{}-{}-{}-{}", A, B, C, D);
}

FUUID FUUID::NewUUID()
{
	std::uniform_int_distribution<uint32> distribution(0, UINT32_MAX);

	FUUID result;
	result.A = distribution(GGenerator);
	result.B = distribution(GGenerator);
	result.C = distribution(GGenerator);
	result.D = distribution(GGenerator);

	return result;
}