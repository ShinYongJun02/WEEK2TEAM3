#pragma once

#include "Core.h"
#include "UEngineStatics.h"
#include "UClass.h"

#define DECLARE_CLASS(ClassName, ParentClass) \
    static UClass* StaticClass() { \
        static UClass Instance(#ClassName, ParentClass::StaticClass(), \
			&ConstructUObject<ClassName>); \
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
