#pragma once

#include "Core.h"

template <typename T>
class TSparseArray
{
public:
	int32 Add(const T& element)
	{
		int32 index;
		if (!FreeIndices.empty())
		{
			index = FreeIndices.back();
			FreeIndices.pop_back();
			Data[index] = element;
			ValidFlags[index] = true;
		}
		else
		{
			index = Data.size();
			Data.push_back(element);
			ValidFlags.push_back(true);
		}

		return index;
	}

	int32 Add(T&& element)
	{
		int32 index;
		if (!FreeIndices.empty())
		{
			index = FreeIndices.back();
			FreeIndices.pop_back();
			Data[index] = std::move(element);
			ValidFlags[index] = true;
		}
		else
		{
			index = Data.size();
			Data.push_back(std::move(element));
			ValidFlags.push_back(true);
		}

		return index;
	}

	void Remove(int32 index)
	{
		ASSERT(IsValid(index));
		Data[index] = {};
		ValidFlags[index] = false;
		FreeIndices.Add(index);
	}

	bool IsValid(int32 index) const
	{
		if (index < 0 || index >= ValidFlags.size())
		{
			return false;
		}

		return ValidFlags[index];
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
