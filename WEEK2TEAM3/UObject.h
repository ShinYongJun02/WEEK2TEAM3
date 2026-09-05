#pragma once

#include "Core.h"
#include "UEngineStatics.h"
#include "UObjectAllocator.h"

#define GENERATED_BODY(ClassType, ParentClassType) \
	public: \
		using Super = ParentClassType; \
		static UObject* CreateInstance() { \
			if constexpr (std::is_abstract_v<ClassType>) { \
				return nullptr; \
			} \
			else { \
				return NewObject<ClassType>(); \
			} \
		} \
		static const UClass* StaticClass() { \
			static UClass ClassInfo{ #ClassType, ParentClassType::StaticClass(), &ClassType::CreateInstance }; \
			return &ClassInfo; \
		} \
		virtual const UClass* GetClass() const override { return ClassType::StaticClass(); } \

class UObject;

extern TArray<UObject*> GUObjectArray;

struct UClass
{
	using CreateFunc = UObject* (*)();

	const char* TypeName = nullptr;
	const UClass* ParentType = nullptr;
	const CreateFunc CreateObject = nullptr;

	bool IsChildOf(const UClass* Other) const
	{
		const UClass* Current = this;
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
};

class UObject
{
public:
	UObject();
	virtual ~UObject();

	template <typename OtherClassType>
	bool IsA() const
	{
		return GetClass()->IsChildOf(OtherClassType::StaticClass());
	}

	static UObject* CreateInstance();
	static const UClass* StaticClass();
	virtual const UClass* GetClass() const;

	FUUID UUID;
	uint32 InternalIndex;
};

template<typename T, typename... Args>
T* NewObject(Args&&... args)
{
	void* memory = FUObjectAllocator::Allocate(sizeof(T), alignof(T));

	T* obj = new (memory) T(std::forward<Args>(args)...);
	obj->UUID = UEngineStatics::GetUUID();
	obj->InternalIndex = (uint32)GUObjectArray.size();
	GUObjectArray.push_back(obj);

	return obj;
}

inline void DeleteObject(UObject* obj)
{
	if (obj)
	{
		obj->~UObject();
		FUObjectAllocator::Deallocate(obj);
	}
}

struct FObjectFactory
{
	static UObject* ConstructObject(const UClass* ClassType)
	{
		if (ClassType->CreateObject)
		{
			return ClassType->CreateObject();
		}
		return nullptr;
	}
};
