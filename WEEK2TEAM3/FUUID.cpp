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
	std::uniform_int_distribution<uint32> Distribution(0, UINT32_MAX);

	A = Distribution(GGenerator);
	B = Distribution(GGenerator);
	C = Distribution(GGenerator);
	D = Distribution(GGenerator);
}

FString FUUID::ToString() const
{
	return std::format("{}-{}-{}-{}", A, B, C, D);
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