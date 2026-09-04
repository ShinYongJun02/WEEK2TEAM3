#pragma once

#include "UEngineStatics.h"

class UObject
{
	public:
	uint32 UUID;
	uint32 InternalIndex;
};

extern TArray<UObject*> GUObjectArray;

template<typename T>
T* NewObject()
{
	T* Object = new T();
	Object->UUID = UEngineStatics::GenUUID();
	Object->InternalIndex = (uint32)GUObjectArray.size();
	GUObjectArray.push_back(Object);
	return (Object);
}
