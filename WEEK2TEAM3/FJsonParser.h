#pragma once

#include "Core.h"

struct FJsonValue
{
	enum class EType
	{
		Number, String, List, Block
	};

	EType Type;

	double Number;
	FString String;
	TArray<FJsonValue> List;
	TArray<TPair<FString, FJsonValue>> Block;

	const FJsonValue* Find(const FString& Key) const;

	float AsFloat() const;
	uint32 AsUInt() const;
	bool AsVector(FVector& Out) const;
	FString AsString() const;
};

class FJsonParser
{
public:
	bool ParseFile(const FString& Path, FJsonValue& Out);

private:
	FString Src;
	size_t Pos = 0;

	void SkipWs();
	char Peek();
	bool Expect(char C);

	// Value := Number | String | List | Block
	bool ParseValue(FJsonValue& Out);
	// Block := '{' [ Member { ',' Member } ] '}'
	bool ParseBlock(FJsonValue& Out);
	// List := '[' [ Token { ',' Token } ] ']'
	bool ParseList(FJsonValue& Out);
	bool ParseString(FString& Out);
	bool ParseNumber(double& Out);
};
