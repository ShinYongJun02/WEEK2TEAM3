#pragma once

#include "Core.h"

template <typename T>
class TSparseArray
{
public:
	int32 Add(const T& Element)
	{
		int32 Index;
		if (!FreeIndices.empty())
		{
			Index = FreeIndices.back();
			FreeIndices.pop_back();
			Data[Index] = Element;
			ValidFlags[Index] = true;
		}
		else
		{
			Index = Data.size();
			Data.push_back(Element);
			ValidFlags.push_back(true);
		}

		return Index;
	}

	int32 Add(T&& Element)
	{
		int32 Index;
		if (!FreeIndices.empty())
		{
			Index = FreeIndices.back();
			FreeIndices.pop_back();
			Data[Index] = std::move(Element);
			ValidFlags[Index] = true;
		}
		else
		{
			Index = Data.size();
			Data.push_back(std::move(Element));
			ValidFlags.push_back(true);
		}

		return Index;
	}

	void Remove(int32 Index)
	{
		ASSERT(IsValid(Index));
		Data[Index] = {};
		ValidFlags[Index] = false;
		FreeIndices.Add(Index);
	}

	bool IsValid(int32 Index) const
	{
		if (Index < 0 || Index >= ValidFlags.size())
		{
			return false;
		}

		return ValidFlags[Index];
	}

	uint32 Size() const
	{
		return Data.size();
	}

private:
	TArray<T> Data;
	TArray<bool> ValidFlags;
	TArray<int32> FreeIndices;
};
