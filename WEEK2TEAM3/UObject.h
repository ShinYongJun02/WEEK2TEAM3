#pragma once

#include "Core.h"
#include "FEngineStatics.h"
#include "FUObjectAllocator.h"

#define GENERATED_BODY(ClassType, ParentClassType) \
	public: \
		using Super = ParentClassType; \
		static UObject* CreateInstance() { \
			if constexpr (std::is_abstract_v<ClassType>) { \
				return nullptr; \
			} \
			else { \
				return FObjectFactory::NewObject<ClassType>(); \
			} \
		} \
		static const FClass* StaticClass() { \
			static FClass ClassInfo{ #ClassType, ParentClassType::StaticClass(), &ClassType::CreateInstance }; \
			return &ClassInfo; \
		} \
		inline static const FClass* AutoRegisterClass = RegisterClass(ClassType::StaticClass()); \
		virtual const FClass* GetClass() const override { return ClassType::StaticClass(); } \

class UObject;

extern TArray<UObject*> GUObjectArray;

struct FClass
{
	using CreateFunc = UObject* (*)();

	const char* TypeName = nullptr;
	const FClass* ParentType = nullptr;
	const CreateFunc CreateObject = nullptr;

	bool IsChildOf(const FClass* Other) const
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
};

// 클래스 이름으로 FClass 를 찾기 위한 전역 레지스트리.
// GENERATED_BODY 의 AutoRegisterClass 가 프로그램 시작 시 자기 자신을 등록한다.
inline TMap<FString, const FClass*>& GetClassRegistry()
{
	static TMap<FString, const FClass*> Registry;
	return Registry;
}

inline const FClass* RegisterClass(const FClass* ClassType)
{
	GetClassRegistry()[ClassType->TypeName] = ClassType;
	return ClassType;
}

inline const FClass* FindClass(const FString& TypeName)
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
	static const FClass* StaticClass();
	virtual const FClass* GetClass() const;

	static void* operator new(size_t Size, std::align_val_t Alignment)
	{
		void* Memory = FUObjectAllocator::Allocate((uint32)Size, (uint32)Alignment);
		if (!Memory)
		{
			throw std::bad_alloc();
		}
		return Memory;
	}

	static void operator delete(void* Ptr, std::align_val_t Alignment) noexcept
	{
		FUObjectAllocator::Deallocate(Ptr);
	}

	static void* operator new(size_t Size)
	{
		return UObject::operator new(Size, static_cast<std::align_val_t>(__STDCPP_DEFAULT_NEW_ALIGNMENT__));
	}

	static void operator delete(void* Ptr) noexcept
	{
		FUObjectAllocator::Deallocate(Ptr);
	}

	FUUID UUID;
	uint32 InternalIndex;
};

struct FObjectFactory
{
	static UObject* ConstructObject(const FClass* ClassType)
	{
		if (ClassType->CreateObject)
		{
			return ClassType->CreateObject();
		}
		return nullptr;
	}

	template<typename T, typename... Args>
	static T* NewObject(Args&&... Arguments)
	{
		T* Obj = new T(std::forward<Args>(Arguments)...);
		Obj->UUID = FEngineStatics::GenUUID();
		Obj->InternalIndex = (uint32)GUObjectArray.size();
		GUObjectArray.push_back(Obj);

		return Obj;
	}
};
