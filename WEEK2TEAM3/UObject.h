#pragma once

#include "UEngineStatics.h"

class UObject
{
	public:
	uint32 UUID;
	uint32 InternalIndex;
public:
	virtual ~UObject() {};
};

extern TArray<UObject*> GUObjectArray;

template<typename T, typename... Args>
T* NewObject(Args&&... args)
{
	T* obj = new T(std::forward<Args>(args)...);
	obj->UUID = UEngineStatics::GenUUID();
	obj->InternalIndex = (uint32)GUObjectArray.size();
	GUObjectArray.push_back(obj);
	return obj;
}
