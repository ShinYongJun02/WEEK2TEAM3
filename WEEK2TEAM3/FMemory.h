#pragma once

#include "Core.h"

class FUObjectAllocator
{
public:
	static void Initialize(uint32 size)
	{
		MemoryPool = malloc(size);
		PoolSize = size;
		FreeList = new MemoryBlock{ MemoryPool, size, nullptr, nullptr };
	}

	static void* Allocate(uint32 size) 
	{
		MemoryBlock* block = FreeList;
		while (block) {
			if (block->Size >= size) {
				void* allocatedPtr = block->Ptr;
				int32 remainingSize = block->Size - size;
				
				if (remainingSize > 0)
				{
					// Adjust the block to reflect the allocated size
					block->Ptr = (char*)block->Ptr + size;
					block->Size = remainingSize;
				}
				else
				{
					// Remove the block from the free list
					if (block == FreeList) {
						// If the block is the head of the free list, update the head
						FreeList = block->Next;
					}
					else {
						// If the block is not the head, find the previous block and update its next pointer
						if (block->Prev) {
							block->Prev->Next = block->Next;
						}
					}
				}

				return allocatedPtr;
			}

			block = block->Next;
		}


	}

	static void Deallocate(void* ptr)
	{

	}

private:
	struct MemoryBlock
	{
		void* Ptr;
		uint32 Size;
		MemoryBlock* Prev;
		MemoryBlock* Next;
	};

	inline static void* MemoryPool = nullptr;
	inline static uint32 PoolSize = 0;
	inline static MemoryBlock* FreeList = nullptr;
};