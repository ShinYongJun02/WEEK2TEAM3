#include "FSceneManager.h"

#include <fstream>

#include "FEngineStatics.h"
#include "FJsonParser.h"
#include "Helper.h"
#include "UObject.h"
#include "UPrimitiveComponent.h"
#include "UResourceManager.h"
#include "USceneComponent.h"

static void WriteVector(std::ofstream& Out, const char* Key, const FVector& Value)
{
	Out << "\t\t\t\"" << Key << "\" : ["
		<< Value.X << ", " << Value.Y << ", " << Value.Z << "],\n";
}

bool FSceneManager::SaveScene(const FString SceneName)
{
	FString Path = SceneName + ".Scene";
	std::ofstream Out(Path);
	if (!Out.is_open())
	{
		return false;
	}

	Out << "{\n";
	Out << "\t\"Version\" : " << SceneVersion << ",\n";
	Out << "\t\"NextUUID\" : \"" << FEngineStatics::GetNextUUID().ToString() << "\",\n";
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

bool FSceneManager::LoadScene(UResourceManager& ResourceManager, FString SceneName)
{
	FJsonParser Parser;
	FJsonValue Root;

	const FString& Path = SceneName + ".Scene";

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

	const FJsonValue* NextUUIDValue = Root.Find("NextUUID");
	if (!NextUUIDValue || NextUUIDValue->Type != FJsonValue::EType::String)
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

		const FClass* ClassType = FindClass(TypeValue->String);
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

		FUUID CurrentUUID = StringToUUID(Member.first);
		Object->UUID = CurrentUUID;
	}

	FEngineStatics::SetNextUUID(StringToUUID(NextUUIDValue->AsString()));

	return true;
}

void FSceneManager::ClearScene()
{
	for (int32 Index = (int32)GUObjectArray.size() - 1; Index >= 0; --Index)
	{
		if (GUObjectArray[Index]->IsA<UPrimitiveComponent>())
		{
			FObjectFactory::DestroyObject(GUObjectArray[Index]);
		}
	}
}
