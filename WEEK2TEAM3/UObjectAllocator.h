#pragma once

#include "Core.h"

class FUObjectAllocator
{
public:
	static void Initialize(uint32 size)
	{
		if (MemoryPool)
		{
			throw std::runtime_error("Memory pool is already initialized.");
		}

		MemoryPool = (int8*)malloc(size);
		PoolSize = size;
		FreeList = new FMemoryBlock{ MemoryPool, size, nullptr, nullptr };
		TotalAllocatedBytes = 0;
		TotalAllocationCount = 0;
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
		TotalAllocatedBytes = 0;
		TotalAllocationCount = 0;
	}

	static void* Allocate(uint32 size, uint32 alignment)
	{
		if (size == 0 || alignment == 0 || (alignment & (alignment - 1)) != 0) {
			return nullptr; // Invalid size or alignment
		}

		const uint64 headerSize = sizeof(FAllocationHeader);

		FMemoryBlock* block = FreeList;
		while (block) {
			FMemory& memory = block->Memory;

			uint64 address = reinterpret_cast<uint64>(memory.Ptr);
			uint64 startAddress = address + headerSize;
			uint32 padding = (alignment - (startAddress % alignment)) % alignment;

			if (headerSize <= memory.Size && padding <= memory.Size - headerSize && size <= memory.Size - headerSize - padding) {
				int8* originalPtr = memory.Ptr;
				int8* alignedPtr = originalPtr + headerSize + padding;
				int8* headerPtr = alignedPtr - headerSize;
				uint32 requiredSize = headerSize + padding + size;
				uint32 remainingSize = memory.Size - requiredSize;

				FAllocationHeader header;
				header.TotalSize = requiredSize;
				header.Offset = alignedPtr - originalPtr;
				std::memcpy(headerPtr, &header, headerSize);

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

				TotalAllocatedBytes += requiredSize;
				TotalAllocationCount++;

				return alignedPtr;
			}

			block = block->Next;
		}

		return nullptr;
	}

	static void Deallocate(void* ptr)
	{
		if (!ptr)
		{
			return;
		}

		const uint64 headerSize = sizeof(FAllocationHeader);

		int8* headerPtr = (int8*)ptr - headerSize;

		FAllocationHeader header;
		std::memcpy(&header, headerPtr, headerSize);
		
		FMemory memory;
		memory.Ptr = (int8*)ptr - header.Offset;
		memory.Size = header.TotalSize;

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

		TotalAllocatedBytes -= header.TotalSize;
		TotalAllocationCount--;
	}

	static uint32 GetHeapSize()
	{
		return PoolSize;
	}

	inline static uint64 GetTotalAllocationBytes()
	{
		return TotalAllocatedBytes;
	}

	inline static uint32 GetTotalAllocationCount()
	{
		return TotalAllocationCount;
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

	struct FAllocationHeader
	{
		uint32 TotalSize; // padding + header + requested size
		uint32 Offset; // offset from the original pointer to returned pointer
	};

	inline static int8* MemoryPool = nullptr;
	inline static uint32 PoolSize = 0;
	inline static FMemoryBlock* FreeList = nullptr;
	inline static uint64 TotalAllocatedBytes = 0;
	inline static uint32 TotalAllocationCount = 0;
};