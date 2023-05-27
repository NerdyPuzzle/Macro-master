#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#include <iostream>
#include <raylib.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <vector>
#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "rlImGui.h"
#include <format>
#include "winutil.h"
#include <map>
#include "icon.h"

namespace fs = std::filesystem;

using json = nlohmann::json;

Image icon = { 0 };

std::string macro_dir = (std::string)getenv("USERPROFILE") + "\\.macromaster";

double elapsedTime = 0;

int recording_fps = 45;
int selected_macro = -1;

bool should_record = false;
bool recording = true;

void SetStyle() {
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 0.0f; // original 5.0
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_TableBorderLight] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}

void LoadIcon() {
	icon.width = ICON_WIDTH;
	icon.height = ICON_HEIGHT;
	icon.format = ICON_FORMAT;
	icon.data = ICON_DATA;
	icon.mipmaps = 1;
}

int main() {
	LoadIcon();
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
    InitWindow(600, 800, "Macro master");
	SetWindowIcon(icon);
    SetWindowMinSize(200, 400);
	SetTargetFPS(60);
	SetExitKey(0);
    rlImGuiSetup(true);
	SetStyle();

	if (!fs::exists(macro_dir))
		fs::create_directory(macro_dir);

    while (!WindowShouldClose()) {
        ClearBackground(BLACK);
        rlImGuiBegin();

		if (recording) {
			double deltaTime = GetFrameTime();

			// Accumulate the elapsed time
			elapsedTime += deltaTime;

			// Check if the desired frame rate is reached
			if (elapsedTime >= 1.0 / recording_fps)
			{
				// Reset the elapsed time
				elapsedTime = 0;

				should_record = true;
			}
		}

        // main menu bar
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Import")) {

				}
				ImGui::Separator();
				if (ImGui::MenuItem("Save")) {

				}
				if (ImGui::MenuItem("Delete")) {

				}
                ImGui::EndMenu();
            }
			if (ImGui::BeginMenu("Macros")) {
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Preferences")) {
				ImGui::EndMenu();
			}
            ImGui::EndMainMenuBar();
        }

        // main window
        ImGui::SetNextWindowPos({ 0, 27 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ (float)GetScreenWidth(), (float)GetScreenHeight() - 27 });
        if (ImGui::Begin("main", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_MenuBar)) {
           
			// menu bar
			if (ImGui::BeginMenuBar()) {
				if (ImGui::Button(std::format(" {} ", ICON_FA_PLAY).c_str())) {

				}
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 10);
				if (ImGui::Button(std::format(" {} ", ICON_FA_CIRCLE).c_str())) {

				}
				ImGui::EndMenuBar();
			}

            ImGui::End();
        }

		should_record = false;

        rlImGuiEnd();
        BeginDrawing();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}