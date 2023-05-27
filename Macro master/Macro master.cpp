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
#include "gui_file_dialogs.h"
#include <fstream>

namespace fs = std::filesystem;

using json = nlohmann::json;

Image icon = { 0 };

std::string macro_dir = (std::string)getenv("USERPROFILE") + "\\.macromaster";

json loaded_macro;
std::vector<json> macros;
std::vector<std::string> macro_names;

double elapsedTime = 0;

// recording state variables
long long int recorded_frames = 0;
std::vector<int> previous_inputs;
std::pair<int, int> previous_mousepos = { -1, -1 };
bool first_frame = false;

// playing state variables
long long int played_frames = 0;
bool loop_frames = false;

int recording_fps = 60;
int selected_macro = -1;

bool recording = false;
bool playing = false;

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

void StartRecording() {
	selected_macro = -1;
	loaded_macro.clear();
	first_frame = true;
	recording = true;
}

void RecordFrame() {
	std::vector<int> inputs = windows::GetCurrentInputs();
	std::pair<int, int> mousepos = windows::GetMousePos();

	if (first_frame) {
		first_frame = false;
		loaded_macro["frame_" + std::to_string(recorded_frames)]["mouse_pos"] = {mousepos.first, mousepos.second};
		if (!inputs.empty())
			loaded_macro["frame_" + std::to_string(recorded_frames)]["inputs_down"] = inputs;
	}
	else {
		if ((mousepos.first != previous_mousepos.first) || (mousepos.second != previous_mousepos.second))
			loaded_macro["frame_" + std::to_string(recorded_frames)]["mouse_pos"] = { mousepos.first, mousepos.second };
		std::vector<int> released_inputs;
		std::vector<int> pressed_inputs;
		for (const int input : inputs) {
			if (std::find(previous_inputs.begin(), previous_inputs.end(), input) == previous_inputs.end())
				pressed_inputs.push_back(input);
		}
		for (const int input : previous_inputs) {
			if (std::find(inputs.begin(), inputs.end(), input) == inputs.end())
				released_inputs.push_back(input);
		}
		if (!pressed_inputs.empty())
			loaded_macro["frame_" + std::to_string(recorded_frames)]["inputs_down"] = pressed_inputs;
		if (!released_inputs.empty())
			loaded_macro["frame_" + std::to_string(recorded_frames)]["inputs_released"] = released_inputs;
	}

	previous_inputs = inputs;
	previous_mousepos = mousepos;

	recorded_frames++;
}

void FinishRecording() {
	recording = false;
	loaded_macro["frames"] = recorded_frames;
	recorded_frames = 0;
	elapsedTime = 0;
	std::ofstream out("test.json");
	out << loaded_macro.dump(2);
	out.close();
}

void PlayFrame(long long int frame) {
	if (loaded_macro.empty())
		return;

	if (loaded_macro.contains("frame_" + std::to_string(frame))) {
		if (loaded_macro.at("frame_" + std::to_string(frame)).contains("mouse_pos"))
			windows::SetMousePos(loaded_macro.at("frame_" + std::to_string(frame)).at("mouse_pos").at(0), loaded_macro.at("frame_" + std::to_string(frame)).at("mouse_pos").at(1));
		if (loaded_macro.at("frame_" + std::to_string(frame)).contains("inputs_released"))
			for (const int input : loaded_macro.at("frame_" + std::to_string(frame)).at("inputs_released"))
				windows::ReleaseKey(input);
		if (loaded_macro.at("frame_" + std::to_string(frame)).contains("inputs_down"))
			for (const int input : loaded_macro.at("frame_" + std::to_string(frame)).at("inputs_down"))
				windows::PressKey(input);
	}
}

int main() {
	LoadIcon();
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
    InitWindow(600, 800, "Macro master");
	SetWindowIcon(icon);
    SetWindowMinSize(200, 400);
	SetTargetFPS(144);
	SetExitKey(60);
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
			if (elapsedTime >= 1.0 / 60)
			{
				// Reset the elapsed time
				elapsedTime = 0;

				RecordFrame();
			}

		}

		if (playing) {
			double deltaTime = GetFrameTime();

			// Accumulate the elapsed time
			elapsedTime += deltaTime;

			// Check if the desired frame rate is reached
			if (elapsedTime >= 1.0 / 60)
			{
				// Reset the elapsed time
				elapsedTime = 0;

				PlayFrame(played_frames);
				played_frames++;

				if (played_frames >= loaded_macro.at("frames")) {
					if (!loop_frames) {
						playing = false;
						played_frames = 0;
					}
					else
						played_frames = 0;
				}
			}
		}

        // main menu bar
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Import")) {
					char path[1024] = { 0 };
					int result = GuiFileDialog(DIALOG_OPEN_FILE, "Import macro", path, "*.macro", "Macro master file (.macro)");
					if (result == 1) {
						std::ifstream macro((std::string)path);
						json m = json::parse(macro);
						if (std::find(macro_names.begin(), macro_names.end(), m.at("name")) == macro_names.end()) {
							macros.push_back(m);
							fs::copy_file((std::string)path, macro_dir + "\\" + (std::string)GetFileName(path));
						}
						macro.close();
					}
				}
				ImGui::Separator();
				if (!recording) {
					if (ImGui::MenuItem("Record", nullptr, nullptr, (!playing))) {
						StartRecording();
					}
				}
				else {
					if (ImGui::MenuItem("Stop recording")) {
						FinishRecording();
					}
				}
				if (ImGui::MenuItem("Play", nullptr, nullptr, (!recording))) {
					if (!recording)
						playing = true;
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Save", nullptr, nullptr, (!playing && !recording))) {

				}
				if (ImGui::MenuItem("Delete", nullptr, nullptr, (!playing && !recording))) {

				}
                ImGui::EndMenu();
            }
			if (ImGui::BeginMenu("Macros", (!playing && !recording))) {
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Preferences", (!playing && !recording))) {
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

        rlImGuiEnd();
        BeginDrawing();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}