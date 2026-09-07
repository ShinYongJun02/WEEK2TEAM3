#pragma once

#include <fstream>

#include "UObject.h"
#include "USceneComponent.h"
#include "FJsonParser.h"
#include "UResourceManager.h"

class USceneManager
{
public:
	static bool SaveScene(uint32 Version, FUUID NextUUID)
	{
		std::ofstream out("Sample.Scene");
		if (!out.is_open()) return false;

		out << "{ \"Version\" : " << Version << ",\n";
		out << "\t\"NextUUID\" : \"" << NextUUID.A << "-" << NextUUID.B << "-" << NextUUID.C << "-" << NextUUID.D << "\",\n";
		out << "\t\"Primitives\" : {\n";

		for (const auto& obj : GUObjectArray)
		{
			const USceneComponent* comp = static_cast<const USceneComponent*>(obj);
			if (obj->InternalIndex != 0)
			{
				out << ",\n";
			}
			out << "\t\t\"" << obj->UUID.A << "-" << obj->UUID.B << "-" << obj->UUID.C << "-" << obj->UUID.D << "\" : {\n";
			out << "\t\t\"Location\" : [";
			out << comp->RelativeLocation.x << ", " << comp->RelativeLocation.y << ", " << comp->RelativeLocation.z << "],\n";
			out << "\t\t\"Rotation\" : [";
			out << comp->RelativeRotation.x << ", " << comp->RelativeRotation.y << ", " << comp->RelativeRotation.z << "],\n";
			out << "\t\t\"Scale\" : [";
			out << comp->RelativeScale3D.x << ", " << comp->RelativeScale3D.y << ", " << comp->RelativeScale3D.z << "],\n";
			out << "\t\t\"Type\" : \"" << comp->GetClass()->GetName() << "\"\n";
			out << "\t\t}";
		}

		out << "\n\t}\n";
		out << "}\n";

		return true;
	}

	static bool LoadScene(UResourceManager& ResourceManager, const FString& Path = "Sample.Scene")
	{
		FJsonParser Parser;
		FJsonValue Root;

		if (!Parser.ParseFile(Path, Root)) return false;

		const FJsonValue* VersionVal = Root.Find("Version");
		if (!VersionVal || VersionVal->Type != FJsonValue::EType::Number) return false;
		/*if (VersionVal->AsUInt() != 1) return false;*/

		const FJsonValue* NextUUIDStr = Root.Find("NextUUID");
		if (!NextUUIDStr || NextUUIDStr->Type != FJsonValue::EType::String) return false;
		// Next UUID 정보 업데이트 ?

		const FJsonValue* Prims = Root.Find("Primitives");
		if (!Prims || Prims->Type != FJsonValue::EType::Block) return false;

		ClearScene();

		for (const auto& [UUIDStr, Obj] : Prims->Block)
		{
			const FJsonValue* TypeVal = Obj.Find("Type");
			if (!TypeVal || TypeVal->Type != FJsonValue::EType::String) continue;

			UClass* Cls = UClass::FindClass(TypeVal->String);
			if (!Cls || !Cls->IsChildOf(USceneComponent::StaticClass())) continue;

			auto* Comp = static_cast<USceneComponent*>(NewObject(Cls));
			if (const FJsonValue* V = Obj.Find("Location")) V->AsVector(Comp->RelativeLocation);
			if (const FJsonValue* V = Obj.Find("Rotation")) V->AsVector(Comp->RelativeRotation);
			if (const FJsonValue* V = Obj.Find("Scale"))    V->AsVector(Comp->RelativeScale3D);
			
			// UUID 복구? 

			if (auto* Prim = dynamic_cast<UPrimitiveComponent*>(Comp))
			{
				static const TMap<FString, FString> MeshKeys = {
					{ "UCubeComp",   "Cube"   },
					{ "USphereComp", "Sphere" },
					{ "UPlaneComp",  "Plane"  },
				};
				auto it = MeshKeys.find(TypeVal->String);
				if (it != MeshKeys.end()) Prim->StaticMesh = ResourceManager.GetStaticMesh(it->second);
			}
		}

		return true;
	}
};
