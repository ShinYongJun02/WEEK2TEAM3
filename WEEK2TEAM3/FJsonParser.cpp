#include "FJsonParser.h"

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <sstream>

const FJsonValue* FJsonValue::Find(const FString& Key) const
{
	for (const auto& M : Block)
	{
		if (M.first == Key) return &M.second;
	}
	return nullptr;
}

float FJsonValue::AsFloat() const
{
	return static_cast<float>(Number);
}

uint32 FJsonValue::AsUInt() const
{
	return static_cast<uint32>(Number);
}

bool FJsonValue::AsVector(FVector& Out) const
{
	if (Type != EType::List || List.size() != 3) return false;
	Out = FVector(List[0].AsFloat(), List[1].AsFloat(), List[2].AsFloat());
	return true;
}

FString FJsonValue::AsString() const
{
	return String;
}

bool FJsonParser::ParseFile(const FString& Path, FJsonValue& Out)
{
	std::ifstream In(Path);
	if (!In.is_open()) return false;

	std::ostringstream Stream;
	Stream << In.rdbuf();
	Src = Stream.str();
	Pos = 0;

	if (!ParseBlock(Out)) return false;
	SkipWs();
	return Pos >= Src.size();   // 뒤에 쓰레기 토큰이 없어야 함
}

void FJsonParser::SkipWs()
{
	while (Pos < Src.size() && std::isspace((unsigned char)Src[Pos])) ++Pos;
}

char FJsonParser::Peek()
{
	SkipWs(); return Pos < Src.size() ? Src[Pos] : '\0';
}

bool FJsonParser::Expect(char C)
{
	if (Peek() != C)
	{
		return false;
	}
	++Pos;
	return true;
}

bool FJsonParser::ParseValue(FJsonValue& Out)
{
	switch (Peek())
	{
	case '{': return ParseBlock(Out);
	case '[': return ParseList(Out);
	case '"': Out.Type = FJsonValue::EType::String; return ParseString(Out.String);
	default:  Out.Type = FJsonValue::EType::Number; return ParseNumber(Out.Number);
	}
}

bool FJsonParser::ParseBlock(FJsonValue& Out)
{
	Out.Type = FJsonValue::EType::Block;
	if (!Expect('{')) return false;
	if (Peek() == '}') return Expect('}');

	do
	{
		FString Key;
		if (!ParseString(Key)) return false;
		if (!Expect(':'))      return false;

		FJsonValue Val;
		if (!ParseValue(Val))  return false;

		Out.Block.emplace_back(std::move(Key), std::move(Val));
	} while (Expect(','));

	return Expect('}');
}

bool FJsonParser::ParseList(FJsonValue& Out)
{
	Out.Type = FJsonValue::EType::List;
	if (!Expect('[')) return false;
	if (Peek() == ']') return Expect(']');

	do
	{
		FJsonValue Token;
		if (Peek() == '"')
		{
			Token.Type = FJsonValue::EType::String;
			if (!ParseString(Token.String)) return false;
		}
		else
		{
			Token.Type = FJsonValue::EType::Number;
			if (!ParseNumber(Token.Number)) return false;
		}
		Out.List.emplace_back(std::move(Token));
	} while (Expect(','));

	return Expect(']');
}

bool FJsonParser::ParseString(FString& Out)
{
	if (!Expect('"')) return false;
	Out.clear();
	while (Pos < Src.size() && Src[Pos] != '"') Out += Src[Pos++];
	return Expect('"');
}

bool FJsonParser::ParseNumber(double& Out)
{
	SkipWs();
	const char* Begin = Src.c_str() + Pos;
	char* End = nullptr;
	Out = std::strtod(Begin, &End);   // int / float 둘 다 처리
	if (End == Begin) return false;
	Pos += static_cast<size_t>(End - Begin);
	return true;
}
