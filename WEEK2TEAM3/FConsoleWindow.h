#pragma once

#include "ImGui/imgui.h"

#include "Core.h"

struct FConsoleWindow
{
	FBitSet<3> LevelFilter;
	ImGuiTextFilter Filter;
	bool bAutoScroll;
	bool bScrollToBottom;

	FConsoleWindow();

	void Draw(const char* Title, bool* bOpen);
};
