#pragma once

#include "Core.h"
#include "UEngineStatics.h"
#include "UClass.h"

#define DECLARE_CLASS(ClassName, ParentClass) \
    static UClass* StaticClass() { \
        static UClass Instance(#ClassName, ParentClass::StaticClass(), \
			MakeConstructor<ClassName>()); \
        return &Instance; \
    } \
	virtual UClass* GetClass() const override { return ClassName::StaticClass(); }

class UObject
{
public:
	FUUID UUID;
	uint32 InternalIndex;

	UObject();
	virtual ~UObject();

	bool IsA(UClass* c) const;
	virtual UClass* GetClass() const;
	static UClass* StaticClass();

	// 메모리 추적용 할당자. 소멸자가 virtual 이므로
	// 기반 포인터로 delete 해도 실제(파생) 타입 크기가 Size 로 넘어온다.
	void* operator new(size_t Size);
	void operator delete(void* Ptr, size_t Size);

	void* operator new[](size_t Size);
	void operator delete[](void* Ptr, size_t Size);
};

extern TArray<UObject*> GUObjectArray;

inline UObject* NewObject(UClass* type)
{
	UObject* obj = type->GetConstructor()();
	obj->UUID = UEngineStatics::GetUUID();
	obj->InternalIndex = (uint32)GUObjectArray.size();
	GUObjectArray.push_back(obj);
	return obj;
}
