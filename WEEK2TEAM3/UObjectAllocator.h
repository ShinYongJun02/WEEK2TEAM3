#pragma once

#include "Core.h"

class FUObjectAllocator
{
public:
	static void Initialize(uint32 size)
	{
		MemoryPool = (int8*)malloc(size);
		PoolSize = size;
		FreeList = new FMemoryBlock{ MemoryPool, size, nullptr, nullptr };
	}

	static void Release()
	{
		free(MemoryPool);
		MemoryPool = nullptr;
		PoolSize = 0;
		FMemoryBlock* block = FreeList;
		while (block) {
			FMemoryBlock* nextBlock = block->Next;
			delete block;
			block = nextBlock;
		}
		FreeList = nullptr;
		AllocatedMemories.clear();
	}

	static void* Allocate(uint32 size, uint32 alignment)
	{
		FMemoryBlock* block = FreeList;
		while (block) {
			FMemory& memory = block->Memory;

			uint64 address = reinterpret_cast<uint64>(memory.Ptr);
			uint32 padding = (alignment - (address % alignment)) % alignment;

			if (padding <= memory.Size && size <= memory.Size - padding) {
				int8* originalPtr = memory.Ptr;
				int8* alignedPtr = originalPtr + padding;
				uint32 requiredSize = size + padding;
				uint32 remainingSize = memory.Size - requiredSize;

				AllocatedMemories.emplace(alignedPtr, FMemory{ originalPtr, requiredSize });

				if (remainingSize > 0)
				{
					memory.Ptr += requiredSize;
					memory.Size = remainingSize;
				}
				else
				{
					if (block->Prev) {
						block->Prev->Next = block->Next;
					}
					else
					{
						FreeList = block->Next;
					}

					if (block->Next) {
						block->Next->Prev = block->Prev;
					}

					delete block;
				}

				return alignedPtr;
			}

			block = block->Next;
		}

		return nullptr;
	}

	static void Deallocate(void* ptr)
	{
		auto it = AllocatedMemories.find(ptr);
		if (it == AllocatedMemories.end())
		{
			return;
		}

		FMemory memory = it->second;
		
		FMemoryBlock* newBlock = new FMemoryBlock{ memory, nullptr, nullptr };
		
		FMemoryBlock* prev = nullptr;
		FMemoryBlock* current = FreeList;
		
		// Find the correct position to insert the new block in the free list
		while (current && current->Memory.Ptr < newBlock->Memory.Ptr)
		{
			prev = current;
			current = current->Next;
		}

		newBlock->Prev = prev;
		newBlock->Next = current;

		if (prev) 
		{
			prev->Next = newBlock;
		} 
		else {
			FreeList = newBlock;
		}

		if (current) {
			current->Prev = newBlock;
		}

		// Merge with previous block if adjacent
		FMemoryBlock* adjacentPrev = newBlock->Prev;
		if (adjacentPrev && adjacentPrev->Memory.Ptr + adjacentPrev->Memory.Size == newBlock->Memory.Ptr)
		{
			adjacentPrev->Memory.Size += newBlock->Memory.Size;
			adjacentPrev->Next = newBlock->Next;
			if (newBlock->Next)
			{
				newBlock->Next->Prev = adjacentPrev;
			}
			delete newBlock;
			newBlock = adjacentPrev;
		}

		// Merge with next block if adjacent
		FMemoryBlock* adjacentNext = newBlock->Next;
		if (adjacentNext && newBlock->Memory.Ptr + newBlock->Memory.Size == adjacentNext->Memory.Ptr)
		{
			newBlock->Memory.Size += adjacentNext->Memory.Size;
			newBlock->Next = adjacentNext->Next;
			if (adjacentNext->Next)
			{
				adjacentNext->Next->Prev = newBlock;
			}
			delete adjacentNext;
		}
	
		AllocatedMemories.erase(it);
	}

	static uint32 GetHeapSize()
	{
		return PoolSize;
	}

	static uint32 GetTotalAllocationBytes()
	{
		uint32 totalAllocated = 0;
		for (const auto& pair : AllocatedMemories)
		{
			totalAllocated += pair.second.Size;
		}
		return totalAllocated;
	}

	static uint32 GetTotalAllocationCount()
	{
		return static_cast<uint32>(AllocatedMemories.size());
	}

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

	inline static int8* MemoryPool = nullptr;
	inline static uint32 PoolSize = 0;
	inline static FMemoryBlock* FreeList = nullptr;
	inline static TMap<void*, FMemory> AllocatedMemories;
};