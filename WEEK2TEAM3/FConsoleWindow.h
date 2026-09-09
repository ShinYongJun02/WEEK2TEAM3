#pragma once

#include "Core.h"
#include "FLogger.h"

struct FConsoleWindow
{
	FBitSet<3> LevelFilter;
	ImGuiTextFilter Filter;
	bool bAutoScroll;
	bool bScrollToBottom;

	FConsoleWindow()
	{
		LevelFilter.set(0, true); // Info
		LevelFilter.set(1, true); // Warning
		LevelFilter.set(2, true); // Error

		bAutoScroll = true;
		bScrollToBottom = false;
	}

	void Draw(const char* Title, bool* bOpen)
	{
		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(Title, bOpen))
		{
			ImGui::End();
			return;
		}

		if (ImGui::SmallButton("Clear")) 
		{ 
			FLogger::ClearLog();
		}

		ImGui::SameLine();
		bool bCopyToClipboard = ImGui::SmallButton("Copy");

		ImGui::Separator();

		// Options menu
		if (ImGui::BeginPopup("Options"))
		{
			ImGui::Checkbox("Auto-scroll", &bAutoScroll);
			
			bool bInfoFilter = LevelFilter.test(0);
			ImGui::Checkbox("Info", &bInfoFilter);
			LevelFilter.set(0, bInfoFilter);

			bool bWarningFilter = LevelFilter.test(1);
			ImGui::Checkbox("Warning", &bWarningFilter);
			LevelFilter.set(1, bWarningFilter);

			bool bErrorFilter = LevelFilter.test(2);
			ImGui::Checkbox("Error", &bErrorFilter);
			LevelFilter.set(2, bErrorFilter);

			ImGui::EndPopup();
		}

		// Options, Filter
		ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_O, ImGuiInputFlags_Tooltip);
		if (ImGui::Button("Options"))
		{
			ImGui::OpenPopup("Options");
		}
		ImGui::SameLine();
		Filter.Draw("##Filter", 180);
		ImGui::Separator();

		// Reserve enough left-over height for 1 separator + 1 input text
		ImGuiStyle& Style = ImGui::GetStyle();
		if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
			if (bCopyToClipboard)
			{
				ImGui::LogToClipboard();
			}

			for (const FLog& Item : FLogger::Logs)
			{
				// 필터 통과 여부 확인
				if (!LevelFilter.test(static_cast<int32>(Item.Level)))
				{
					continue;
				}

				if (!Filter.PassFilter(Item.Message.c_str()))
				{
					continue;
				}

				ImVec4 Color;
				bool bHasColor = false;

				if (Item.Level == ELogLevel::Warning)
				{
					Color = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
					bHasColor = true;
				}
				else if (Item.Level == ELogLevel::Error) 
				{ 
					Color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
					bHasColor = true; 
				}

				if (bHasColor)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, Color);
				}

				ImGui::TextUnformatted(Item.Message.c_str());

				if (bHasColor)
				{
					ImGui::PopStyleColor();
				}
			}

			if (bCopyToClipboard)
			{
				ImGui::LogFinish();
			}

			if (bScrollToBottom || (bAutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			{
				ImGui::SetScrollHereY(1.0f);
			}
			bScrollToBottom = false;

			ImGui::PopStyleVar();
		}

		ImGui::EndChild();

		ImGui::End();
	}
};