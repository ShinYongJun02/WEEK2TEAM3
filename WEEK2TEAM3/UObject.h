#pragma once

#include <new>
#include <type_traits>

#include "Core.h"
#include "FEngineStatics.h"
#include "FUUID.h"

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
		inline static const FClass* AutoRegisterClass = RegisterClassType(ClassType::StaticClass()); \
		virtual const FClass* GetClass() const override { return ClassType::StaticClass(); } \

class UObject;

extern TArray<UObject*> GUObjectArray;

struct FClass
{
	using CreateFunc = UObject* (*)();

	const char* TypeName = nullptr;
	const FClass* ParentType = nullptr;
	const CreateFunc CreateObject = nullptr;

	bool IsChildOf(const FClass* Other) const;
};

// 클래스 이름으로 FClass 를 찾기 위한 전역 레지스트리.
// GENERATED_BODY 의 AutoRegisterClass 가 프로그램 시작 시 자기 자신을 등록한다.
TMap<FString, const FClass*>& GetClassRegistry();
const FClass* RegisterClassType(const FClass* ClassType);
const FClass* FindClass(const FString& TypeName);

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

	static void* operator new(size_t Size, std::align_val_t Alignment);
	static void operator delete(void* Ptr, std::align_val_t Alignment) noexcept;
	static void* operator new(size_t Size);
	static void operator delete(void* Ptr) noexcept;

	FUUID UUID;
	uint32 InternalIndex;
};

struct FObjectFactory
{
	static UObject* ConstructObject(const FClass* ClassType);

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
