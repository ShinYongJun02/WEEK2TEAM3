#include "UObject.h"

#include "FUObjectAllocator.h"

TArray<UObject*> GUObjectArray;

bool FClass::IsChildOf(const FClass* Other) const
{
	const FClass* Current = this;
	while (Current)
	{
		if (Current == Other)
		{
			return true;
		}
		Current = Current->ParentType;
	}
	return false;
}

TMap<FString, const FClass*>& GetClassRegistry()
{
	static TMap<FString, const FClass*> Registry;
	return Registry;
}

const FClass* RegisterClassType(const FClass* ClassType)
{
	GetClassRegistry()[ClassType->TypeName] = ClassType;
	return ClassType;
}

const FClass* FindClass(const FString& TypeName)
{
	auto It = GetClassRegistry().find(TypeName);
	return It == GetClassRegistry().end() ? nullptr : It->second;
}

UObject::UObject()
	: UUID()
	, InternalIndex(0)
{
}

UObject::~UObject()
{
	if (InternalIndex < GUObjectArray.size() && GUObjectArray[InternalIndex] == this)
	{
		GUObjectArray[InternalIndex] = GUObjectArray.back();
		GUObjectArray[InternalIndex]->InternalIndex = InternalIndex;
		GUObjectArray.pop_back();
	}
}

UObject* UObject::CreateInstance()
{
	return FObjectFactory::NewObject<UObject>();
}

const FClass* UObject::StaticClass()
{
	static FClass ClassInfo{ "UObject", nullptr, &UObject::CreateInstance };
	return &ClassInfo;
}

const FClass* UObject::GetClass() const
{
	return UObject::StaticClass();
}

void* UObject::operator new(size_t Size, std::align_val_t Alignment)
{
	void* Memory = FUObjectAllocator::Allocate((uint32)Size, (uint32)Alignment);
	if (!Memory)
	{
		throw std::bad_alloc();
	}
	return Memory;
}

void UObject::operator delete(void* Ptr, std::align_val_t Alignment) noexcept
{
	FUObjectAllocator::Deallocate(Ptr);
}

void* UObject::operator new(size_t Size)
{
	return UObject::operator new(Size, static_cast<std::align_val_t>(__STDCPP_DEFAULT_NEW_ALIGNMENT__));
}

void UObject::operator delete(void* Ptr) noexcept
{
	FUObjectAllocator::Deallocate(Ptr);
}

UObject* FObjectFactory::ConstructObject(const FClass* ClassType)
{
	if (ClassType->CreateObject)
	{
		return ClassType->CreateObject();
	}
	return nullptr;
}
