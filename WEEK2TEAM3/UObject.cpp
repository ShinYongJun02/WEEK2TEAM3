#pragma once
#include "UObject.h"

UObject::UObject() : UUID(0), InternalIndex(0) {}

UObject::~UObject()
{
	if (!GUObjectArray.empty() && !(GUObjectArray[InternalIndex] == this))
	{
		GUObjectArray[InternalIndex] = GUObjectArray.back();
		GUObjectArray[InternalIndex]->InternalIndex = InternalIndex;
		GUObjectArray.pop_back();
		//GUObjectArray.erase(GUObjectArray.begin() + InternalIndex);
	}
};

uint32 UEngineStatics::NextUUID = 0;

TArray<UObject*> GUObjectArray;
//TArray<std::unique_ptr<UObject>> GUObjectArray;


