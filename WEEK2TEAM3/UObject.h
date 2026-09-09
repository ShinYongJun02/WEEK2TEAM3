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
		inline static const UClass* AutoRegisterClass = RegisterClass(ClassType::StaticClass()); \
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

// 클래스 이름으로 UClass 를 찾기 위한 전역 레지스트리.
// GENERATED_BODY 의 AutoRegisterClass 가 프로그램 시작 시 자기 자신을 등록한다.
inline TMap<FString, const UClass*>& GetClassRegistry()
{
	static TMap<FString, const UClass*> Registry;
	return Registry;
}

inline const UClass* RegisterClass(const UClass* ClassType)
{
	GetClassRegistry()[ClassType->TypeName] = ClassType;
	return ClassType;
}

inline const UClass* FindClass(const FString& TypeName)
{
	auto It = GetClassRegistry().find(TypeName);
	return It == GetClassRegistry().end() ? nullptr : It->second;
}

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

	static void* operator new(size_t size, std::align_val_t alignment)
	{
		void* memory = FUObjectAllocator::Allocate((uint32)size, (uint32)alignment);
		if (!memory)
		{
			throw std::bad_alloc();
		}
		return memory;
	}

	static void operator delete(void* ptr, std::align_val_t alignment) noexcept
	{
		FUObjectAllocator::Deallocate(ptr);
	}

	static void* operator new(size_t size)
	{
		return UObject::operator new(size, static_cast<std::align_val_t>(__STDCPP_DEFAULT_NEW_ALIGNMENT__));
	}

	static void operator delete(void* ptr) noexcept
	{
		FUObjectAllocator::Deallocate(ptr);
	}

	FUUID UUID;
	uint32 InternalIndex;
};

template<typename T, typename... Args>
T* NewObject(Args&&... args)
{
	T* obj = new T(std::forward<Args>(args)...);
	obj->UUID = UEngineStatics::GenUUID();
	obj->InternalIndex = (uint32)GUObjectArray.size();
	GUObjectArray.push_back(obj);

	return obj;
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
