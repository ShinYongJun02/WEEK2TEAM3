#pragma once

#include <fstream>

#include "UObject.h"
#include "USceneComponent.h"

class SceneManager
{
	// 테스트용
	bool SaveScene(uint32 Version, FUUID NextUUID)
	{
		std::ofstream out("Sample.Scene");
		if (!out.is_open()) return false;

		out << "{ \"Version\" : " << Version << ",\n";
		out << "\t\"NextUUID\" : " << NextUUID.A << NextUUID.B << NextUUID.C << NextUUID.D << ",\n";
		out << "\t\"Primitives\" : {\n";

		for (const auto& obj : GUObjectArray)
		{
			const USceneComponent* comp = static_cast<const USceneComponent*>(obj);
			out << "\t  \"" << obj->UUID.A << obj->UUID.B << obj->UUID.C << obj->UUID.D << "\" : {\n";
			out << "\t  \"Location\" : [";
			out << comp->RelativeLocation.x << ", " << comp->RelativeLocation.y << ", " << comp->RelativeLocation.z << "],\n";
			out << "\t  \"Rotation\" : [";
			out << comp->RelativeRotation.x << ", " << comp->RelativeRotation.y << ", " << comp->RelativeRotation.z << "],\n";
			out << "\t  \"Scale\" : [";
			out << comp->RelativeScale3D.x << ", " << comp->RelativeScale3D.y << ", " << comp->RelativeScale3D.z << "],\n";
			out << "\t  \"Type\" : \"" /*<< comp->GetClass()*/ << "\"\n";
			out << "\t  },\n";
		}

		out << "\t}\n";
		out << "}\n";

		return true;
	}
};