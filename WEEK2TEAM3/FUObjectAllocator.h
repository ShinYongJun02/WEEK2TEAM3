#pragma once

#include "Core.h"

class FUObjectAllocator
{
public:
	static void Initialize(uint32 Size);
	static void Release();

	static void* Allocate(uint32 Size, uint32 Alignment);
	static void Deallocate(void* Ptr);

	static uint32 GetHeapSize();
	static uint64 GetTotalAllocationBytes();
	static uint32 GetTotalAllocationCount();

private:
	struct FMemory
	{
		int8* Ptr;
		uint32 Size;
	};

	struct FMemoryBlock
	{
		FMemory Memory;
		FMemoryBlock* Prev;
		FMemoryBlock* Next;
	};

	struct FAllocationHeader
	{
		uint32 TotalSize; // padding + header + requested size
		uint32 Offset; // offset from the original pointer to returned pointer
	};

	static int8* MemoryPool;
	static uint32 PoolSize;
	static FMemoryBlock* FreeList;
	static uint64 TotalAllocatedBytes;
	static uint32 TotalAllocationCount;
};
