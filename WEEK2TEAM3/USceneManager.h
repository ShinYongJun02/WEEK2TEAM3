#pragma once

#include <fstream>

#include "UObject.h"
#include "USceneComponent.h"
#include "UPrimitiveComponent.h"
#include "UResourceManager.h"
#include "FJsonParser.h"

class USceneManager
{
public:
	static constexpr uint32 SceneVersion = 1;

	static bool SaveScene(const FString& Path = "Sample.Scene")
	{
		std::ofstream Out(Path);
		if (!Out.is_open())
		{
			return false;
		}

		Out << "{\n";
		Out << "\t\"Version\" : " << SceneVersion << ",\n";
		Out << "\t\"NextUUID\" : \"" << UEngineStatics::GetUUID().ToString() << "\",\n";
		Out << "\t\"Primitives\" : {\n";

		bool bFirst = true;
		for (const UObject* Object : GUObjectArray)
		{
			if (!Object->IsA<UPrimitiveComponent>())
			{
				continue;
			}

			const USceneComponent* Component = static_cast<const USceneComponent*>(Object);

			if (!bFirst)
			{
				Out << ",\n";
			}
			bFirst = false;

			Out << "\t\t\"" << Object->UUID.ToString() << "\" : {\n";
			WriteVector(Out, "Location", Component->RelativeLocation);
			WriteVector(Out, "Rotation", Component->RelativeRotation);
			WriteVector(Out, "Scale", Component->RelativeScale3D);
			Out << "\t\t\t\"Type\" : \"" << Object->GetClass()->TypeName << "\"\n";
			Out << "\t\t}";
		}

		Out << "\n\t}\n";
		Out << "}\n";

		return true;
	}

	static bool LoadScene(UResourceManager& ResourceManager, const FString& Path = "Sample.Scene")
	{
		FJsonParser Parser;
		FJsonValue Root;

		if (!Parser.ParseFile(Path, Root))
		{
			return false;
		}

		// 파서는 문법만 본다. 의미 검증은 여기서 한다.
		const FJsonValue* VersionValue = Root.Find("Version");
		if (!VersionValue || VersionValue->Type != FJsonValue::EType::Number)
		{
			return false;
		}
		if (VersionValue->AsUInt() != SceneVersion)
		{
			return false;
		}

		const FJsonValue* PrimitivesValue = Root.Find("Primitives");
		if (!PrimitivesValue || PrimitivesValue->Type != FJsonValue::EType::Block)
		{
			return false;
		}

		ClearScene();

		for (const TPair<FString, FJsonValue>& Member : PrimitivesValue->Block)
		{
			const FJsonValue& ObjectValue = Member.second;
			if (ObjectValue.Type != FJsonValue::EType::Block)
			{
				continue;
			}

			const FJsonValue* TypeValue = ObjectValue.Find("Type");
			if (!TypeValue || TypeValue->Type != FJsonValue::EType::String)
			{
				continue;
			}

			const UClass* ClassType = FindClass(TypeValue->String);
			if (!ClassType || !ClassType->IsChildOf(UPrimitiveComponent::StaticClass()))
			{
				continue;
			}

			// 추상 클래스면 CreateObject 가 nullptr 을 돌려준다.
			UObject* Object = FObjectFactory::ConstructObject(ClassType);
			if (!Object)
			{
				continue;
			}

			UPrimitiveComponent* Primitive = static_cast<UPrimitiveComponent*>(Object);
			Primitive->Initialize(ResourceManager);

			if (const FJsonValue* Value = ObjectValue.Find("Location"))
			{
				Value->AsVector(Primitive->RelativeLocation);
			}
			if (const FJsonValue* Value = ObjectValue.Find("Rotation"))
			{
				Value->AsVector(Primitive->RelativeRotation);
			}
			if (const FJsonValue* Value = ObjectValue.Find("Scale"))
			{
				Value->AsVector(Primitive->RelativeScale3D);
			}
		}

		return true;
	}

	// 카메라처럼 GUObjectArray 밖에서 관리되는 객체를 건드리지 않도록
	// 프리미티브만 지운다. 소멸자가 swap-remove 를 하므로 뒤에서부터 돈다.
	static void ClearScene()
	{
		for (int32 Index = (int32)GUObjectArray.size() - 1; Index >= 0; --Index)
		{
			if (GUObjectArray[Index]->IsA<UPrimitiveComponent>())
			{
				delete GUObjectArray[Index];
			}
		}
	}

private:
	static void WriteVector(std::ofstream& Out, const char* Key, const FVector& Value)
	{
		Out << "\t\t\t\"" << Key << "\" : ["
			<< Value.x << ", " << Value.y << ", " << Value.z << "],\n";
	}
};
