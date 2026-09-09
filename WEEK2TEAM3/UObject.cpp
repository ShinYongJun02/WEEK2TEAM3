#include "UObject.h"

TArray<UObject*> GUObjectArray;

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
};

UObject* UObject::CreateInstance()
{
	return FObjectFactory::NewObject<UObject>();
}

const FClass* UObject::StaticClass() {
	static FClass ClassInfo{ "UObject", nullptr, &UObject::CreateInstance };
	return &ClassInfo;
}

const FClass* UObject::GetClass() const {
	return UObject::StaticClass();
}

