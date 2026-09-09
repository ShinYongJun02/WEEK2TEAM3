#pragma once

#include "Core.h"
#include "FLogger.h"

struct FConsoleWindow
{
	FBitSet<3> LevelFilter;
	ImGuiTextFilter Filter;
	bool AutoScroll;
	bool ScrollToBottom;

	FConsoleWindow()
	{
		LevelFilter.set(0, true); // Info
		LevelFilter.set(1, true); // Warning
		LevelFilter.set(2, true); // Error

		AutoScroll = true;
		ScrollToBottom = false;
	}

	void Draw(const char* Title, bool* Open)
	{
		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(Title, Open))
		{
			ImGui::End();
			return;
		}

		if (ImGui::SmallButton("Clear")) 
		{ 
			FLogger::ClearLog();
		}

		ImGui::SameLine();
		bool CopyToClipboard = ImGui::SmallButton("Copy");

		ImGui::Separator();

		// Options menu
		if (ImGui::BeginPopup("Options"))
		{
			ImGui::Checkbox("Auto-scroll", &AutoScroll);
			
			bool InfoFilter = LevelFilter.test(0);
			ImGui::Checkbox("Info", &InfoFilter);
			LevelFilter.set(0, InfoFilter);

			bool WarningFilter = LevelFilter.test(1);
			ImGui::Checkbox("Warning", &WarningFilter);
			LevelFilter.set(1, WarningFilter);

			bool ErrorFilter = LevelFilter.test(2);
			ImGui::Checkbox("Error", &ErrorFilter);
			LevelFilter.set(2, ErrorFilter);

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
			if (CopyToClipboard)
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
				bool HasColor = false;

				if (Item.Level == ELogLevel::Warning)
				{
					Color = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
					HasColor = true;
				}
				else if (Item.Level == ELogLevel::Error) 
				{ 
					Color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
					HasColor = true; 
				}

				if (HasColor)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, Color);
				}

				ImGui::TextUnformatted(Item.Message.c_str());

				if (HasColor)
				{
					ImGui::PopStyleColor();
				}
			}

			if (CopyToClipboard)
			{
				ImGui::LogFinish();
			}

			if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			{
				ImGui::SetScrollHereY(1.0f);
			}
			ScrollToBottom = false;

			ImGui::PopStyleVar();
		}

		ImGui::EndChild();

		ImGui::End();
	}
};