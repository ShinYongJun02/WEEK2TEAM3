#include "UObject.h"

UObject::UObject() : UUID(0), InternalIndex(0) {}

UObject::~UObject()
{
	if (InternalIndex < GUObjectArray.size() && GUObjectArray[InternalIndex] == this)
	{
		GUObjectArray[InternalIndex] = GUObjectArray.back();
		GUObjectArray[InternalIndex]->InternalIndex = InternalIndex;
		GUObjectArray.pop_back();
		//GUObjectArray.erase(GUObjectArray.begin() + InternalIndex);
	}
}

bool UObject::IsA(UClass* c) const
{
	return GetClass()->IsChildOf(c);
}

UClass* UObject::GetClass() const
{
	return StaticClass();
}

UClass* UObject::StaticClass()
{
	static UClass Instance("UObject", nullptr, &ConstructUObject<UObject>);
	return &Instance;
}

TArray<UObject*> GUObjectArray;
//TArray<std::unique_ptr<UObject>> GUObjectArray;
