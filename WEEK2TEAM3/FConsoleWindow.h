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

	void Draw(const char* title, bool* p_open)
	{
		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(title, p_open))
		{
			ImGui::End();
			return;
		}

		if (ImGui::SmallButton("Clear")) 
		{ 
			FLogger::ClearLog();
		}

		ImGui::SameLine();
		bool copy_to_clipboard = ImGui::SmallButton("Copy");

		ImGui::Separator();

		// Options menu
		if (ImGui::BeginPopup("Options"))
		{
			ImGui::Checkbox("Auto-scroll", &AutoScroll);
			
			bool infoFilter = LevelFilter.test(0);
			ImGui::Checkbox("Info", &infoFilter);
			LevelFilter.set(0, infoFilter);

			bool warningFilter = LevelFilter.test(1);
			ImGui::Checkbox("Warning", &warningFilter);
			LevelFilter.set(1, warningFilter);

			bool errorFilter = LevelFilter.test(2);
			ImGui::Checkbox("Error", &errorFilter);
			LevelFilter.set(2, errorFilter);

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
		ImGuiStyle& style = ImGui::GetStyle();
		if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
			if (copy_to_clipboard)
			{
				ImGui::LogToClipboard();
			}

			for (const FLog& item : FLogger::Logs)
			{
				// 필터 통과 여부 확인
				if (!LevelFilter.test(static_cast<int32>(item.Level)))
				{
					continue;
				}

				if (!Filter.PassFilter(item.Message.c_str()))
				{
					continue;
				}

				ImVec4 color;
				bool has_color = false;

				if (item.Level == ELogLevel::Warning)
				{
					color = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
					has_color = true;
				}
				else if (item.Level == ELogLevel::Error) 
				{ 
					color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
					has_color = true; 
				}

				if (has_color)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, color);
				}

				ImGui::TextUnformatted(item.Message.c_str());

				if (has_color)
				{
					ImGui::PopStyleColor();
				}
			}

			if (copy_to_clipboard)
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