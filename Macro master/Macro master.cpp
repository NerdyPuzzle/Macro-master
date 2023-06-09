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
#include "gui_file_dialogs.h"
#include <fstream>
#include <imgui_internal.h>
#include "icon.h"

namespace fs = std::filesystem;

using json = nlohmann::json;

Image icon = { 0 };

std::string macro_dir = (std::string)getenv("USERPROFILE") + "\\.macromaster";
std::string macroname;

json loaded_macro;
std::vector<json> macros;
std::vector<std::string> macro_names;

json settings;

double elapsedTime = 0;

// recording state variables
long long int recorded_frames = 0;
std::vector<int> previous_inputs;
std::pair<int, int> previous_mousepos = { -1, -1 };
bool first_frame = false;
bool ignore_delays = false;
bool ignore_mouse = false;

// playing state variables
long long int played_frames = 0;
bool loop_frames = false;

int recording_fps = 60;
int playing_fps = 60;
int selected_macro = -1;

// keybinds
int keybind_import = 77;
int keybind_record = 70;
int keybind_play = 71;
int keybind_save = 67;
int keybind_delete = 87;

bool recording = false;
bool wait = false;
bool playing = false;
bool save = false;
bool save_set_pos = true;
bool keybinds = false;
bool keybinds_set_pos = true;
bool settings_recording = false;
bool settings_recording_set_pos = true;
bool settings_playback = false;
bool settings_playback_set_pos = true;

ImVec4 ConvertColor(Color color) {
	return ImVec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
}

void ToggleButton(const char* str_id, bool* v, float size) {
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float height = size;
	float width = height * 1.55f;
	float radius = height * 0.50f;

	ImGui::InvisibleButton(str_id, ImVec2(width, height));
	if (ImGui::IsItemClicked())
		*v = !*v;

	float t = *v ? 1.0f : 0.0f;

	ImGuiContext& g = *GImGui;
	float ANIM_SPEED = 0.08f;
	if (g.LastActiveId == g.CurrentWindow->GetID(str_id))// && g.LastActiveIdTimer < ANIM_SPEED)
	{
		float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
		t = *v ? (t_anim) : (1.0f - t_anim);
	}

	ImU32 col_bg;
	if (ImGui::IsItemHovered())
		col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), ImVec4(0.64f, 0.83f, 0.34f, 1.0f), t)); // Hovered inactive, Hovered active
	else
		col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImVec4(0.56f, 0.83f, 0.26f, 1.0f), t)); // ??, ??

	draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
	draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}

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
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.11f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.11f, 0.09f, 0.12f, 1.00f);
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
	style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.14f, 0.12f, 0.13f, 0.73f);
}

void LoadIcon() {
	icon.width = ICON_WIDTH;
	icon.height = ICON_HEIGHT;
	icon.format = ICON_FORMAT;
	icon.data = ICON_DATA;
	icon.mipmaps = 1;
}

void LoadSettings() {
	std::ifstream in(macro_dir + "\\settings.json");
	if (in.is_open()) {
		settings = json::parse(in);
		loop_frames = settings.at("playback").at("should_loop");
		playing_fps = settings.at("playback").at("framerate");
		ignore_delays = settings.at("recording").at("ignore_delays");
		recording_fps = settings.at("recording").at("framerate");
		// check if the setting exists to prevent previous versions from crashing on load
		if (settings.at("recording").contains("ignore_mouse"))
			ignore_mouse = settings.at("recording").at("ignore_mouse");
		keybind_import = settings.at("keybinds").at("import");
		keybind_record = settings.at("keybinds").at("record");
		keybind_play = settings.at("keybinds").at("play");
		keybind_save = settings.at("keybinds").at("save");
		keybind_delete = settings.at("keybinds").at("delete");
	}
	in.close();
}

void SaveSettings() {
	settings["playback"]["should_loop"] = loop_frames;
	settings["playback"]["framerate"] = playing_fps;
	settings["recording"]["ignore_delays"] = ignore_delays;
	settings["recording"]["framerate"] = recording_fps;
	settings["recording"]["ignore_mouse"] = ignore_mouse;
	settings["keybinds"]["import"] = keybind_import;
	settings["keybinds"]["record"] = keybind_record;
	settings["keybinds"]["play"] = keybind_play;
	settings["keybinds"]["save"] = keybind_save;
	settings["keybinds"]["delete"] = keybind_delete;
	std::ofstream out(macro_dir + "\\settings.json");
	out << settings.dump(2);
	out.close();
}

void StartRecording() {
	selected_macro = -1;
	loaded_macro.clear();
	first_frame = true;
	recording = true;
}

void RecordFrame() {
	std::vector<int> inputs = windows::GetCurrentInputs();
	std::pair<int, int> mousepos = { 0, 0 };
	if (!ignore_mouse);
		mousepos = windows::GetMousePos();

	if (first_frame) {
		first_frame = false;
		if (!ignore_mouse)
			loaded_macro["frame_" + std::to_string(recorded_frames)]["mouse_pos"] = {mousepos.first, mousepos.second};
		if (!inputs.empty())
			loaded_macro["frame_" + std::to_string(recorded_frames)]["inputs_down"] = inputs;
	}
	else {
		bool mouse_empty = false;
		bool presses_empty = false;
		bool releases_empty = false;
		if (!ignore_mouse) {
			if ((mousepos.first != previous_mousepos.first) || (mousepos.second != previous_mousepos.second))
				loaded_macro["frame_" + std::to_string(recorded_frames)]["mouse_pos"] = { mousepos.first, mousepos.second };
			else
				mouse_empty = true;
		}
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
		else
			presses_empty = true;
		if (!released_inputs.empty())
			loaded_macro["frame_" + std::to_string(recorded_frames)]["inputs_released"] = released_inputs;
		else
			releases_empty = true;
		if (ignore_delays && (mouse_empty || ignore_mouse) && presses_empty && releases_empty)
			recorded_frames--;
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
	SetTargetFPS(60);
	SetExitKey(0);
    rlImGuiSetup(true);
	SetStyle();

	if (!fs::exists(macro_dir))
		fs::create_directories(macro_dir + "\\macros");

	for (const fs::path entry : fs::directory_iterator(macro_dir + "\\macros")) {
		if (fs::is_regular_file(entry) && entry.filename().extension().string() == ".macro") {
			std::ifstream macro(entry);
			json file = json::parse(macro);
			macro_names.push_back(file.at("name"));
			macro.close();
			macros.push_back(file);
		}
	}

	if (fs::exists(macro_dir + "\\settings.json")) {
		LoadSettings();
	}
	else {
		SaveSettings();
	}

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

				RecordFrame();
			}

		}

		if (playing) {

			double deltaTime = GetFrameTime();

			// Accumulate the elapsed time
			elapsedTime += deltaTime;

			// Check if the desired frame rate is reached
			if (elapsedTime >= 1.0 / playing_fps)
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
				std::string shortcut = "(Ctrl + " + windows::GetInputName(keybind_import) + ")";
				if (ImGui::MenuItem("Import", shortcut.c_str(), nullptr, (!playing && !recording))) {
					char path[1024] = { 0 };
					int result = GuiFileDialog(DIALOG_OPEN_FILE, "Import macro", path, "*.macro", "Macro master file (.macro)");
					if (result == 1) {
						std::ifstream macro((std::string)path);
						json m = json::parse(macro);
						if (std::find(macro_names.begin(), macro_names.end(), m.at("name")) == macro_names.end()) {
							macros.push_back(m);
							macro_names.push_back(m.at("name"));
							fs::copy_file((std::string)path, macro_dir + "\\macros\\" + (std::string)GetFileName(path));
						}
						macro.close();
					}
				}
				ImGui::Separator();
				if (!recording) {
					std::string shortcut = "(Ctrl + " + windows::GetInputName(keybind_record) + ")";
					if (ImGui::MenuItem("Record", shortcut.c_str(), nullptr, (!playing))) {
						if (!wait)
							StartRecording();
						else
							wait = false;
					}
				}
				else {
					std::string shortcut = "(Ctrl + " + windows::GetInputName(keybind_record) + ")";
					if (ImGui::MenuItem("Stop recording", shortcut.c_str())) {
						FinishRecording();
					}
				}
				if (!playing) {
					std::string shortcut = "(Ctrl + " + windows::GetInputName(keybind_play) + ")";
					if (ImGui::MenuItem("Play", shortcut.c_str(), nullptr, (!recording))) {
						if (!recording && !loaded_macro.empty()) {
							playing = true;
							wait = true;
						}
					}
				}
				else {
					std::string shortcut = "(Ctrl + " + windows::GetInputName(keybind_play) + ")";
					if (ImGui::MenuItem("Stop playing", shortcut.c_str())) {
						played_frames = 0;
						playing = false;
						wait = false;
					}
				}
				ImGui::Separator();
				std::string shortcut_ = "(Ctrl + " + windows::GetInputName(keybind_save) + ")";
				if (ImGui::MenuItem("Save", shortcut_.c_str(), nullptr, (!playing && !recording))) {
					if (!loaded_macro.empty()) {
						if (!loaded_macro.contains("name"))
							save = true;
						else {
							std::ofstream out(macro_dir + "\\macros\\" + (std::string)loaded_macro.at("name") + ".macro");
							out << loaded_macro.dump(2);
							out.close();
						}
					}
				}
				std::string shortcut__ = "(Ctrl + " + windows::GetInputName(keybind_delete) + ")";
				if (ImGui::MenuItem("Delete", shortcut__.c_str(), nullptr, (!playing && !recording))) {
					if (selected_macro != -1) {
						if (loaded_macro.contains("name")) {
							fs::remove(macro_dir + "\\macros\\" + (std::string)loaded_macro["name"] + ".macro");
							macros.erase(macros.begin() + selected_macro);
							macro_names.erase(macro_names.begin() + selected_macro);
							selected_macro = -1;
							loaded_macro.clear();
						}
					}
					else if (!loaded_macro.empty())
						loaded_macro.clear();
				}
                ImGui::EndMenu();
            }
			if (ImGui::BeginMenu("Macros", (!playing && !recording))) {
				for (int i = 0; i < macros.size(); i++) {
					bool selected = selected_macro == i;
					if (ImGui::Selectable(macro_names[i].c_str(), &selected)) {
						selected_macro = i;
						loaded_macro = macros[i];
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Preferences", (!playing && !recording))) {
				if (ImGui::MenuItem("Keybinds")) {
					keybinds = true;
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Recording")) {
					settings_recording = true;
				}
				if (ImGui::MenuItem("Playback")) {
					settings_playback = true;
				}
				ImGui::EndMenu();
			}
            ImGui::EndMainMenuBar();
        }

		// save macro dialog
		if (save) {
			if (save_set_pos) {
				save_set_pos = false;
				ImGui::OpenPopup("Save macro");
				ImGui::SetNextWindowPos({ (float)(GetScreenWidth() - 300) / 2, (float)(GetScreenHeight() - 133) / 2 });
				ImGui::SetNextWindowSize({ 300, 133 });
			}
			if (ImGui::BeginPopupModal("Save macro", &save, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Macro name:");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
				ImGui::InputText(" ", &macroname);
				ImGui::Spacing();
				ImGui::SetCursorPosX(65);
				bool duplicate = std::find(macro_names.begin(), macro_names.end(), macroname) != macro_names.end();
				if (ImGui::Button("Save", { 75, 30 }) && !duplicate && !macroname.empty()) {
					save = false;
					loaded_macro["name"] = macroname;
					std::ofstream out(macro_dir + "\\macros\\" + macroname + ".macro");
					out << loaded_macro.dump(2);
					out.close();
					macros.push_back(loaded_macro);
					macro_names.push_back(macroname);
					selected_macro = macros.size() - 1;
				}
				if (duplicate && ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					std::string msg = "A macro with the name " + macroname + " already exists!";
					ImGui::Text(msg.c_str());
					ImGui::EndTooltip();
				}
				ImGui::SameLine();
				ImGui::SetCursorPosX(160);
				if (ImGui::Button("Cancel", { 75, 30 }))
					save = false;
				ImGui::End();
			}
		}
		else {
			if (!macroname.empty())
				macroname.clear();
			save_set_pos = true;
		}

		// keybind settings
		if (keybinds) {
			if (keybinds_set_pos) {
				keybinds_set_pos = false;
				ImGui::OpenPopup("Keybind settings");
				ImGui::SetNextWindowPos({ (float)(GetScreenWidth() - 335) / 2, (float)(GetScreenHeight() - 270) / 2 });
				ImGui::SetNextWindowSize({ 335, 270 });
			}
			if (ImGui::BeginPopupModal("Keybind settings", &keybinds, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Import keybind: Ctrl +");
				ImGui::SameLine();
				ImGui::PushID(2);
				ImGui::Button(windows::GetInputName(keybind_import).c_str());
				ImGui::PopID();
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("Click a key.");
					ImGui::EndTooltip();
					std::vector<int> keys = windows::GetCurrentInputs();
					if (!keys.empty())
						keybind_import = keys[0];
				}
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Record keybind: Ctrl +");
				ImGui::SameLine();
				ImGui::PushID(3);
				ImGui::Button(windows::GetInputName(keybind_record).c_str());
				ImGui::PopID();
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("Click a key.");
					ImGui::EndTooltip();
					std::vector<int> keys = windows::GetCurrentInputs();
					if (!keys.empty())
						keybind_record = keys[0];
				}
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Play keybind: Ctrl +");
				ImGui::SameLine();
				ImGui::PushID(4);
				ImGui::Button(windows::GetInputName(keybind_play).c_str());
				ImGui::PopID();
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("Click a key.");
					ImGui::EndTooltip();
					std::vector<int> keys = windows::GetCurrentInputs();
					if (!keys.empty())
						keybind_play = keys[0];
				}
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Save keybind: Ctrl +");
				ImGui::SameLine();
				ImGui::PushID(5);
				ImGui::Button(windows::GetInputName(keybind_save).c_str());
				ImGui::PopID();
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("Click a key.");
					ImGui::EndTooltip();
					std::vector<int> keys = windows::GetCurrentInputs();
					if (!keys.empty())
						keybind_save = keys[0];
				}
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Delete keybind: Ctrl +");
				ImGui::SameLine();
				ImGui::PushID(6);
				ImGui::Button(windows::GetInputName(keybind_delete).c_str());
				ImGui::PopID();
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("Click a key.");
					ImGui::EndTooltip();
					std::vector<int> keys = windows::GetCurrentInputs();
					if (!keys.empty())
						keybind_delete = keys[0];
				}
				ImGui::SetCursorPosX(73.75);
				if (ImGui::Button("Save", { 83.75, 30 })) {
					SaveSettings();
					keybinds = false;
					keybinds_set_pos = true;
				}
				ImGui::SameLine();
				ImGui::SetCursorPosX(177.5);
				if (ImGui::Button("Cancel", { 83.75, 30 })) {
					LoadSettings();
					keybinds = false;
					keybinds_set_pos = true;
				}
				ImGui::End();
			}
		}
		else {
			if (!keybinds_set_pos) {
				keybinds_set_pos = true;
				LoadSettings();
			}
		}

		// recording settings
		if (settings_recording) {
			if (settings_recording_set_pos) {
				settings_recording_set_pos = false;
				ImGui::OpenPopup("Recording settings");
				ImGui::SetNextWindowPos({ (float)(GetScreenWidth() - 300) / 2, (float)(GetScreenHeight() - 185) / 2 });
				ImGui::SetNextWindowSize({ 300, 185 });
			}
			if (ImGui::BeginPopupModal("Recording settings", &settings_recording, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Recording framerate:");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
				ImGui::SliderInt(" ", &recording_fps, 1, 60);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1);
				ImGui::Text("Ignore empty frames:");
				ImGui::SameLine();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 1);
				ToggleButton("frames_ignr", &ignore_delays, 22);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("Ignore mouse movement:");
				ImGui::SameLine();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
				ToggleButton("mouse_ignr", &ignore_mouse, 22);
				ImGui::SetCursorPosX(65);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1);
				if (ImGui::Button("Save", { 75, 30 })) {
					SaveSettings();
					settings_recording = false;
					settings_recording_set_pos = true;
				}
				ImGui::SameLine();
				ImGui::SetCursorPosX(160);
				if (ImGui::Button("Cancel", { 75, 30 })) {
					LoadSettings();
					settings_recording = false;
					settings_recording_set_pos = true;
				}
				ImGui::End();
			}
		}
		else {
			if (!settings_recording_set_pos) {
				settings_recording_set_pos = true;
				LoadSettings();
			}
		}

		// playback settings
		if (settings_playback) {
			if (settings_playback_set_pos) {
				settings_playback_set_pos = false;
				ImGui::OpenPopup("Playback settings");
				ImGui::SetNextWindowPos({ (float)(GetScreenWidth() - 300) / 2, (float)(GetScreenHeight() - 155) / 2 });
				ImGui::SetNextWindowSize({ 300, 155 });
			}
			if (ImGui::BeginPopupModal("Playback settings", &settings_playback, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Playback framerate:");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
				ImGui::SliderInt(" ", &playing_fps, 1, 60);
				ImGui::Text("Loop after finishing:");
				ImGui::SameLine();
				ToggleButton("loop", &loop_frames, 22);
				ImGui::SetCursorPosX(65);
				if (ImGui::Button("Save", { 75, 30 })) {
					SaveSettings();
					settings_playback = false;
					settings_playback_set_pos = true;
				}
				ImGui::SameLine();
				ImGui::SetCursorPosX(160);
				if (ImGui::Button("Cancel", { 75, 30 })) {
					LoadSettings();
					settings_playback = false;
					settings_playback_set_pos = true;
				}
				ImGui::End();
			}
		}
		else {
			if (!settings_playback_set_pos) {
				settings_playback_set_pos = true;
				LoadSettings();
			}
		}

        // main window
        ImGui::SetNextWindowPos({ 0, 27 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ (float)GetScreenWidth(), (float)GetScreenHeight() - 27 });
        if (ImGui::Begin("main", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_MenuBar)) {
           
			// menu bar
			if (ImGui::BeginMenuBar()) {
				if (playing || recording) {
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}
				if (ImGui::Button(std::format(" {} ", ICON_FA_PLAY).c_str())) {
					if (!recording && !loaded_macro.empty()) {
						playing = true;
						wait = true;
					}
				}
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 10);
				ImGui::PushID(3);
				if (ImGui::Button(std::format(" {} ", ICON_FA_CIRCLE).c_str())) {
					if (!wait)
						StartRecording();
					else
						wait = false;
				}
				ImGui::PopID();
				if (playing || recording) {
					ImGui::PopStyleVar();
				}
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 10);
				ImGui::PushID(2);
				if (ImGui::Button(std::format(" {} ", ICON_FA_STOP).c_str())) {
					if (recording)
						FinishRecording();
					else if (playing) {
						played_frames = 0;
						playing = false;
						wait = false;
					}
				}
				ImGui::PopID();
				if (playing || recording) {
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}
				ImGui::PushID(4);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 10);
				if (ImGui::Button(std::format(" {} ", ICON_FA_FLOPPY_DISK).c_str())) {
					if (!loaded_macro.empty()) {
						if (!loaded_macro.contains("name"))
							save = true;
						else {
							std::ofstream out(macro_dir + "\\macros\\" + (std::string)loaded_macro.at("name") + ".macro");
							out << loaded_macro.dump(2);
							out.close();
						}
					}
				}
				ImGui::PopID();
				if (playing || recording) {
					ImGui::PopStyleVar();
				}
				ImGui::EndMenuBar();
			}

			if (!loaded_macro.empty() && loaded_macro.contains("frames")) {
				if (ImGui::BeginTable("macro", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY)) {
					ImGui::TableSetupScrollFreeze(4, 1);
					ImGui::TableSetupColumn("Frame", ImGuiTableColumnFlags_NoHide, 0.5);
					ImGui::AlignTextToFramePadding();
					ImGui::Text("Frame");
					ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_NoHide);
					ImGui::AlignTextToFramePadding();
					ImGui::Text("Action");
					ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_NoHide);
					ImGui::AlignTextToFramePadding();
					ImGui::Text("Value");
					ImGui::TableSetupColumn(" ", ImGuiTableColumnFlags_NoHide, 0.2);
					ImGui::TableHeadersRow();
					int id = 0;
					for (long long int i = 0; i < loaded_macro["frames"]; i++) {
						if (loaded_macro.contains("frame_" + std::to_string(i))) {
							if (loaded_macro.at("frame_" + std::to_string(i)).contains("mouse_pos")) {
								ImGui::TableNextRow();
								ImGui::TableSetColumnIndex(0);
								ImGui::AlignTextToFramePadding();
								ImGui::Text(std::to_string(i).c_str());
								ImGui::TableSetColumnIndex(1);
								ImGui::AlignTextToFramePadding();
								ImGui::Text(std::format("{} Mouse movement", ICON_FA_I_CURSOR).c_str());
								ImGui::TableSetColumnIndex(2);
								ImGui::AlignTextToFramePadding();
								std::string pos = "( " + std::to_string(loaded_macro.at("frame_" + std::to_string(i)).at("mouse_pos").at(0).get<int>()) + ", " + std::to_string(loaded_macro.at("frame_" + std::to_string(i)).at("mouse_pos").at(1).get<int>()) + " )";
								ImGui::Text(pos.c_str());
								ImGui::TableSetColumnIndex(3);
								ImGui::PushID(id);
								id++;
								if (ImGui::Button(std::format("{}", ICON_FA_XMARK).c_str(), { ImGui::GetColumnWidth(), 28 })) {
									loaded_macro["frame_" + std::to_string(i)].erase(loaded_macro["frame_" + std::to_string(i)].find("mouse_pos"));
								}
								ImGui::PopID();
							}
							if (loaded_macro.at("frame_" + std::to_string(i)).contains("inputs_released")) {
								for (int l = 0; l < loaded_macro.at("frame_" + std::to_string(i)).at("inputs_released").size(); l++) {
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::AlignTextToFramePadding();
									ImGui::Text(std::to_string(i).c_str());
									ImGui::TableSetColumnIndex(1);
									ImGui::AlignTextToFramePadding();
									ImGui::Text(std::format("{} Key release", ICON_FA_CLIPBOARD).c_str());
									ImGui::TableSetColumnIndex(2);
									ImGui::AlignTextToFramePadding();
									ImGui::Text(windows::GetInputName(loaded_macro.at("frame_" + std::to_string(i)).at("inputs_released").at(l)).c_str());
									ImGui::TableSetColumnIndex(3);
									ImGui::PushID(id);
									id++;
									if (ImGui::Button(std::format("{}", ICON_FA_XMARK).c_str(), { ImGui::GetColumnWidth(), 28 })) {
										auto path = json::json_pointer{ "/frame_" + std::to_string(i) + "/inputs_released" };
										auto& file = loaded_macro[path];
										file.erase(file.begin() + l);
									}
									ImGui::PopID();
								}
							}
							if (loaded_macro.at("frame_" + std::to_string(i)).contains("inputs_down")) {
								for (int l = 0; l < loaded_macro.at("frame_" + std::to_string(i)).at("inputs_down").size(); l++) {
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::AlignTextToFramePadding();
									ImGui::Text(std::to_string(i).c_str());
									ImGui::TableSetColumnIndex(1);
									ImGui::AlignTextToFramePadding();
									ImGui::Text(std::format("{} Key press", ICON_FA_CLIPBOARD).c_str());
									ImGui::TableSetColumnIndex(2);
									ImGui::AlignTextToFramePadding();
									ImGui::Text(windows::GetInputName(loaded_macro.at("frame_" + std::to_string(i)).at("inputs_down").at(l)).c_str());
									ImGui::TableSetColumnIndex(3);
									ImGui::PushID(id);
									id++;
									if (ImGui::Button(std::format("{}", ICON_FA_XMARK).c_str(), { ImGui::GetColumnWidth(), 28 })) {
										auto path = json::json_pointer{ "/frame_" + std::to_string(i) + "/inputs_down" };
										auto& file = loaded_macro[path];
										file.erase(file.begin() + l);
									}
									ImGui::PopID();
								}
							}
						}
					}
					ImGui::EndTable();
				}
			}

			// shortcuts
			if (windows::InputIsHeld(17) && windows::InputIsHeld(keybind_import)) {
				windows::ReleaseKey(keybind_import);
				char path[1024] = { 0 };
				int result = GuiFileDialog(DIALOG_OPEN_FILE, "Import macro", path, "*.macro", "Macro master file (.macro)");
				if (result == 1) {
					std::ifstream macro((std::string)path);
					json m = json::parse(macro);
					if (std::find(macro_names.begin(), macro_names.end(), m.at("name")) == macro_names.end()) {
						macros.push_back(m);
						macro_names.push_back(m.at("name"));
						fs::copy_file((std::string)path, macro_dir + "\\macros\\" + (std::string)GetFileName(path));
					}
					macro.close();
				}
			}
			else if (windows::InputIsHeld(17) && windows::InputIsHeld(keybind_record)) {
				windows::ReleaseKey(keybind_record);
				if (!recording) {
					if (!wait)
						StartRecording();
					else
						wait = false;
				}
				else {
					FinishRecording();
				}
			}
			else if (windows::InputIsHeld(17) && windows::InputIsHeld(keybind_play)) {
				windows::ReleaseKey(keybind_play);
				if (!playing) {
					if (!recording && !loaded_macro.empty()) {
						playing = true;
						wait = true;
					}
				}
				else {
					played_frames = 0;
					playing = false;
					wait = false;
				}
			}
			else if (windows::InputIsHeld(17) && windows::InputIsHeld(keybind_save)) {
				windows::ReleaseKey(keybind_save);
				if (!loaded_macro.empty()) {
					if (!loaded_macro.contains("name"))
						save = true;
					else {
						std::ofstream out(macro_dir + "\\macros\\" + (std::string)loaded_macro.at("name") + ".macro");
						out << loaded_macro.dump(2);
						out.close();
					}
				}
			}
			else if (windows::InputIsHeld(17) && windows::InputIsHeld(keybind_delete)) {
				windows::ReleaseKey(keybind_delete);
				if (selected_macro != -1) {
					if (loaded_macro.contains("name")) {
						fs::remove(macro_dir + "\\macros\\" + (std::string)loaded_macro["name"] + ".macro");
						macros.erase(macros.begin() + selected_macro);
						macro_names.erase(macro_names.begin() + selected_macro);
						selected_macro = -1;
						loaded_macro.clear();
					}
				}
				else if (!loaded_macro.empty())
					loaded_macro.clear();
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