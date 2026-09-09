#pragma once

#include "Core.h"

class UResourceManager;

class FSceneManager
{
public:
	static constexpr uint32 SceneVersion = 1;

	static bool SaveScene(const FString SceneName);
	static bool LoadScene(UResourceManager& ResourceManager, FString SceneName);

	// 카메라처럼 GUObjectArray 밖에서 관리되는 객체를 건드리지 않도록
	// 프리미티브만 지운다. 소멸자가 swap-remove 를 하므로 뒤에서부터 돈다.
	static void ClearScene();
};
