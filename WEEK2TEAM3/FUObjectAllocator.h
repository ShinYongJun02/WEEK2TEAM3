#pragma once

#include "Core.h"

class FUObjectAllocator
{
public:
	static void Initialize(uint32 Size)
	{
		if (MemoryPool)
		{
			throw std::runtime_error("Memory pool is already initialized.");
		}

		MemoryPool = (int8*)malloc(Size);
		PoolSize = Size;
		FreeList = new FMemoryBlock{ MemoryPool, Size, nullptr, nullptr };
		TotalAllocatedBytes = 0;
		TotalAllocationCount = 0;
	}

	static void Release()
	{
		free(MemoryPool);
		MemoryPool = nullptr;
		PoolSize = 0;
		FMemoryBlock* Block = FreeList;
		while (Block) {
			FMemoryBlock* NextBlock = Block->Next;
			delete Block;
			Block = NextBlock;
		}
		FreeList = nullptr;
		TotalAllocatedBytes = 0;
		TotalAllocationCount = 0;
	}

	static void* Allocate(uint32 Size, uint32 Alignment)
	{
		if (Size == 0 || Alignment == 0 || (Alignment & (Alignment - 1)) != 0) {
			return nullptr; // Invalid size or alignment
		}

		const uint64 HeaderSize = sizeof(FAllocationHeader);

		FMemoryBlock* Block = FreeList;
		while (Block) {
			FMemory& Memory = Block->Memory;

			uint64 Address = reinterpret_cast<uint64>(Memory.Ptr);
			uint64 StartAddress = Address + HeaderSize;
			uint32 Padding = (Alignment - (StartAddress % Alignment)) % Alignment;

			if (HeaderSize <= Memory.Size && Padding <= Memory.Size - HeaderSize && Size <= Memory.Size - HeaderSize - Padding) {
				int8* OriginalPtr = Memory.Ptr;
				int8* AlignedPtr = OriginalPtr + HeaderSize + Padding;
				int8* HeaderPtr = AlignedPtr - HeaderSize;
				uint32 RequiredSize = HeaderSize + Padding + Size;
				uint32 RemainingSize = Memory.Size - RequiredSize;

				FAllocationHeader Header;
				Header.TotalSize = RequiredSize;
				Header.Offset = AlignedPtr - OriginalPtr;
				std::memcpy(HeaderPtr, &Header, HeaderSize);

				if (RemainingSize > 0)
				{
					Memory.Ptr += RequiredSize;
					Memory.Size = RemainingSize;
				}
				else
				{
					if (Block->Prev) {
						Block->Prev->Next = Block->Next;
					}
					else
					{
						FreeList = Block->Next;
					}

					if (Block->Next) {
						Block->Next->Prev = Block->Prev;
					}

					delete Block;
				}

				TotalAllocatedBytes += RequiredSize;
				TotalAllocationCount++;

				return AlignedPtr;
			}

			Block = Block->Next;
		}

		return nullptr;
	}

	static void Deallocate(void* Ptr)
	{
		if (!Ptr)
		{
			return;
		}

		const uint64 HeaderSize = sizeof(FAllocationHeader);

		int8* HeaderPtr = (int8*)Ptr - HeaderSize;

		FAllocationHeader Header;
		std::memcpy(&Header, HeaderPtr, HeaderSize);
		
		FMemory Memory;
		Memory.Ptr = (int8*)Ptr - Header.Offset;
		Memory.Size = Header.TotalSize;

		FMemoryBlock* NewBlock = new FMemoryBlock{ Memory, nullptr, nullptr };
		
		FMemoryBlock* Prev = nullptr;
		FMemoryBlock* Current = FreeList;
		
		// Find the correct position to insert the new block in the free list
		while (Current && Current->Memory.Ptr < NewBlock->Memory.Ptr)
		{
			Prev = Current;
			Current = Current->Next;
		}

		NewBlock->Prev = Prev;
		NewBlock->Next = Current;

		if (Prev) 
		{
			Prev->Next = NewBlock;
		} 
		else {
			FreeList = NewBlock;
		}

		if (Current) {
			Current->Prev = NewBlock;
		}

		// Merge with previous block if adjacent
		FMemoryBlock* AdjacentPrev = NewBlock->Prev;
		if (AdjacentPrev && AdjacentPrev->Memory.Ptr + AdjacentPrev->Memory.Size == NewBlock->Memory.Ptr)
		{
			AdjacentPrev->Memory.Size += NewBlock->Memory.Size;
			AdjacentPrev->Next = NewBlock->Next;
			if (NewBlock->Next)
			{
				NewBlock->Next->Prev = AdjacentPrev;
			}
			delete NewBlock;
			NewBlock = AdjacentPrev;
		}

		// Merge with next block if adjacent
		FMemoryBlock* AdjacentNext = NewBlock->Next;
		if (AdjacentNext && NewBlock->Memory.Ptr + NewBlock->Memory.Size == AdjacentNext->Memory.Ptr)
		{
			NewBlock->Memory.Size += AdjacentNext->Memory.Size;
			NewBlock->Next = AdjacentNext->Next;
			if (AdjacentNext->Next)
			{
				AdjacentNext->Next->Prev = NewBlock;
			}
			delete AdjacentNext;
		}

		TotalAllocatedBytes -= Header.TotalSize;
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