#pragma once

#include "Core.h"

class UObject;
using ConstructorType = UObject * (*)();

template<typename T>
inline UObject* ConstructUObject()
{
	return new T();
}

// 클래스 이름 변경? UObject 상속?
class UClass
{
private:
	FString ClassName = "";
	UClass* ParentClass = nullptr;
	ConstructorType Constructor = nullptr;

public:
	UClass(const FString& InName, UClass* InParent, ConstructorType InConstructor)
		:ClassName(InName), ParentClass(InParent), Constructor(InConstructor) {
	}

	const FString& GetName() const
	{
		return ClassName;
	}

	UClass* GetParent() const
	{
		return ParentClass;
	}

	ConstructorType GetConstructor() const
	{
		return Constructor;
	}

	bool IsChildOf(const UClass* target) const
	{
		for (const UClass* C = this; C; C = C->GetParent())
		{
			if (C == target)
			{
				return true;
			}
		}
		return false;
	}
};
