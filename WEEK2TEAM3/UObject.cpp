#include "UObject.h"

TArray<UObject*> GUObjectArray;

UObject::UObject() 
	: UUID(0)
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
};

UObject* UObject::CreateInstance()
{
	return NewObject<UObject>();
}

const UClass* UObject::StaticClass() {
	static UClass ClassInfo{ "UObject", nullptr, &UObject::CreateInstance };
	return &ClassInfo;
}

const UClass* UObject::GetClass() const {
	return UObject::StaticClass();
}

