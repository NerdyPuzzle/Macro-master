#pragma once
#include<string>
#include<vector>

namespace windows {

	std::vector<int> GetCurrentInputs();
	std::string GetInputName(int key);
	bool InputIsHeld(int key);
	std::pair<int, int> GetMousePos();

	void SetMousePos(int x, int y);
	void PressKey(int key);
	void ReleaseKey(int key);

}

