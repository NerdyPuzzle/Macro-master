#include "winutil.h"
#include <windows.h>

std::vector<int> windows::GetCurrentInputs() {
    std::vector<int> keys;
    for (int key = 0; key < 256; key++) {
        if (GetAsyncKeyState(key) & 0x8000) {
            // Key is pressed
            keys.push_back(key);
        }
    }
    return keys;
}

std::string windows::GetInputName(int key) {
    switch (key) {
    case 0x00: return "";
    case VK_LBUTTON: return "Left Mouse Button";
    case VK_RBUTTON: return "Right Mouse Button";
    case VK_CANCEL: return "Control-Break";
    case VK_MBUTTON: return "Middle Mouse Button";
    case VK_XBUTTON1: return "X1 Mouse Button";
    case VK_XBUTTON2: return "X2 Mouse Button";
    case VK_BACK: return "Backspace";
    case VK_TAB: return "Tab";
    case VK_CLEAR: return "Clear";
    case VK_RETURN: return "Enter";
    case VK_SHIFT: return "Shift";
    case VK_CONTROL: return "Ctrl";
    case VK_MENU: return "Alt";
    case VK_PAUSE: return "Pause";
    case VK_CAPITAL: return "Caps Lock";
    case VK_KANA: return "Kana";
    case VK_JUNJA: return "Junja";
    case VK_FINAL: return "Final";
    case VK_HANJA: return "Hanja";
    case VK_ESCAPE: return "Esc";
    case VK_CONVERT: return "Convert";
    case VK_NONCONVERT: return "Nonconvert";
    case VK_ACCEPT: return "Accept";
    case VK_MODECHANGE: return "Mode Change";
    case VK_SPACE: return "Space";
    case VK_PRIOR: return "Page Up";
    case VK_NEXT: return "Page Down";
    case VK_END: return "End";
    case VK_HOME: return "Home";
    case VK_LEFT: return "Left Arrow";
    case VK_UP: return "Up Arrow";
    case VK_RIGHT: return "Right Arrow";
    case VK_DOWN: return "Down Arrow";
    case VK_SELECT: return "Select";
    case VK_PRINT: return "Print";
    case VK_EXECUTE: return "Execute";
    case VK_SNAPSHOT: return "Print Screen";
    case VK_INSERT: return "Insert";
    case VK_DELETE: return "Delete";
    case VK_HELP: return "Help";
    case 0x30: return "0";
    case 0x31: return "1";
    case 0x32: return "2";
    case 0x33: return "3";
    case 0x34: return "4";
    case 0x35: return "5";
    case 0x36: return "6";
    case 0x37: return "7";
    case 0x38: return "8";
    case 0x39: return "9";
    case 0x41: return "A";
    case 0x42: return "B";
    case 0x43: return "C";
    case 0x44: return "D";
    case 0x45: return "E";
    case 0x46: return "F";
    case 0x47: return "G";
    case 0x48: return "H";
    case 0x49: return "I";
    case 0x4A: return "J";
    case 0x4B: return "K";
    case 0x4C: return "L";
    case 0x4D: return "M";
    case 0x4E: return "N";
    case 0x4F: return "O";
    case 0x50: return "P";
    case 0x51: return "Q";
    case 0x52: return "R";
    case 0x53: return "S";
    case 0x54: return "T";
    case 0x55: return "U";
    case 0x56: return "V";
    case 0x57: return "W";
    case 0x58: return "X";
    case 0x59: return "Y";
    case 0x5A: return "Z";
    case VK_LWIN: return "Left Windows";
    case VK_RWIN: return "Right Windows";
    case VK_APPS: return "Applications";
    case VK_SLEEP: return "Sleep";
    case VK_NUMPAD0: return "Numpad 0";
    case VK_NUMPAD1: return "Numpad 1";
    case VK_NUMPAD2: return "Numpad 2";
    case VK_NUMPAD3: return "Numpad 3";
    case VK_NUMPAD4: return "Numpad 4";
    case VK_NUMPAD5: return "Numpad 5";
    case VK_NUMPAD6: return "Numpad 6";
    case VK_NUMPAD7: return "Numpad 7";
    case VK_NUMPAD8: return "Numpad 8";
    case VK_NUMPAD9: return "Numpad 9";
    case VK_MULTIPLY: return "Numpad *";
    case VK_ADD: return "Numpad +";
    case VK_SEPARATOR: return "Separator";
    case VK_SUBTRACT: return "Numpad -";
    case VK_DECIMAL: return "Numpad .";
    case VK_DIVIDE: return "Numpad /";
    case VK_F1: return "F1";
    case VK_F2: return "F2";
    case VK_F3: return "F3";
    case VK_F4: return "F4";
    case VK_F5: return "F5";
    case VK_F6: return "F6";
    case VK_F7: return "F7";
    case VK_F8: return "F8";
    case VK_F9: return "F9";
    case VK_F10: return "F10";
    case VK_F11: return "F11";
    case VK_F12: return "F12";
    case VK_F13: return "F13";
    case VK_F14: return "F14";
    case VK_F15: return "F15";
    case VK_F16: return "F16";
    case VK_F17: return "F17";
    case VK_F18: return "F18";
    case VK_F19: return "F19";
    case VK_F20: return "F20";
    case VK_F21: return "F21";
    case VK_F22: return "F22";
    case VK_F23: return "F23";
    case VK_F24: return "F24";
    case VK_NUMLOCK: return "Num Lock";
    case VK_SCROLL: return "Scroll Lock";
    case VK_OEM_NEC_EQUAL: return "OEM NEC Equal";
    case VK_OEM_FJ_MASSHOU: return "OEM FJ Masshou";
    case VK_OEM_FJ_TOUROKU: return "OEM FJ Touroku";
    case VK_OEM_FJ_LOYA: return "OEM FJ Loya";
    case VK_OEM_FJ_ROYA: return "OEM FJ Roya";
    case VK_LSHIFT: return "Left Shift";
    case VK_RSHIFT: return "Right Shift";
    case VK_LCONTROL: return "Left Ctrl";
    case VK_RCONTROL: return "Right Ctrl";
    case VK_LMENU: return "Left Alt";
    case VK_RMENU: return "Right Alt";
    case VK_BROWSER_BACK: return "Browser Back";
    case VK_BROWSER_FORWARD: return "Browser Forward";
    case VK_BROWSER_REFRESH: return "Browser Refresh";
    case VK_BROWSER_STOP: return "Browser Stop";
    case VK_BROWSER_SEARCH: return "Browser Search";
    case VK_BROWSER_FAVORITES: return "Browser Favorites";
    case VK_BROWSER_HOME: return "Browser Home";
    case VK_VOLUME_MUTE: return "Volume Mute";
    case VK_VOLUME_DOWN: return "Volume Down";
    case VK_VOLUME_UP: return "Volume Up";
    case VK_MEDIA_NEXT_TRACK: return "Media Next Track";
    case VK_MEDIA_PREV_TRACK: return "Media Previous Track";
    case VK_MEDIA_STOP: return "Media Stop";
    case VK_MEDIA_PLAY_PAUSE: return "Media Play/Pause";
    case VK_LAUNCH_MAIL: return "Launch Mail";
    case VK_LAUNCH_MEDIA_SELECT: return "Launch Media Select";
    case VK_LAUNCH_APP1: return "Launch App 1";
    case VK_LAUNCH_APP2: return "Launch App 2";
    case VK_OEM_1: return ";";
    case VK_OEM_PLUS: return "+";
    case VK_OEM_COMMA: return ",";
    case VK_OEM_MINUS: return "-";
    case VK_OEM_PERIOD: return ".";
    case VK_OEM_2: return "/";
    case VK_OEM_3: return "`";
    case VK_OEM_4: return "[";
    case VK_OEM_5: return "\\";
    case VK_OEM_6: return "]";
    case VK_OEM_7: return "'";
    case VK_OEM_8: return "OEM 8";
    case VK_OEM_AX: return "AX";
    case VK_OEM_102: return "OEM 102";
    case VK_ICO_HELP: return "ICO Help";
    case VK_ICO_00: return "ICO 00";
    case VK_PROCESSKEY: return "Process Key";
    case VK_ICO_CLEAR: return "ICO Clear";
    case VK_PACKET: return "Packet";
    case VK_OEM_RESET: return "OEM Reset";
    case VK_OEM_JUMP: return "OEM Jump";
    case VK_OEM_PA1: return "OEM PA1";
    case VK_OEM_PA2: return "OEM PA2";
    case VK_OEM_PA3: return "OEM PA3";
    case VK_OEM_WSCTRL: return "OEM WSCTRL";
    case VK_OEM_CUSEL: return "OEM CUSEL";
    case VK_OEM_ATTN: return "OEM ATTN";
    case VK_OEM_FINISH: return "OEM Finish";
    case VK_OEM_COPY: return "OEM Copy";
    case VK_OEM_AUTO: return "OEM Auto";
    case VK_OEM_ENLW: return "OEM ENLW";
    case VK_OEM_BACKTAB: return "OEM Back Tab";
    case VK_ATTN: return "Attn";
    case VK_CRSEL: return "CrSel";
    case VK_EXSEL: return "ExSel";
    case VK_EREOF: return "Erase EOF";
    case VK_PLAY: return "Play";
    case VK_ZOOM: return "Zoom";
    case VK_NONAME: return "NoName";
    case VK_PA1: return "PA1";
    case VK_OEM_CLEAR: return "OEM Clear";
    default: return "Unknown Key";
    }
}

bool windows::InputIsHeld(int key) {
    return (GetAsyncKeyState(key) & 0x8000);
}

std::pair<int, int> windows::GetMousePos() {
    POINT point;
    if (GetCursorPos(&point))
    {
        return std::make_pair(point.x, point.y);
    }
    else
    {
        // Failed to get mouse position
        return std::make_pair(-1, -1);
    }
}

void windows::SetMousePos(int x, int y) {
    SetCursorPos(x, y);
}

void windows::PressKey(int key) {
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = static_cast<WORD>(key);
    input.ki.wScan = 0;  // Use MapVirtualKey to get the scan code
    input.ki.dwFlags = 0; // 0 for key press
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;

    if (key >= VK_LBUTTON && key <= VK_XBUTTON2) {
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

        if (key == VK_RBUTTON) {
            input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
        }
        else if (key == VK_MBUTTON) {
            input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
        }
        else if (key >= VK_XBUTTON1 && key <= VK_XBUTTON2) {
            int buttonOffset = key - VK_XBUTTON1;
            input.mi.dwFlags = MOUSEEVENTF_XDOWN | (buttonOffset << 2);
        }
    }

    // Simulate key press or mouse button press
    SendInput(1, &input, sizeof(INPUT));
}

void windows::ReleaseKey(int key) {
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = static_cast<WORD>(key);
    input.ki.wScan = 0;  // Use MapVirtualKey to get the scan code
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;

    if (key >= VK_LBUTTON && key <= VK_XBUTTON2) {
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;

        if (key == VK_RBUTTON) {
            input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
        }
        else if (key == VK_MBUTTON) {
            input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
        }
        else if (key >= VK_XBUTTON1 && key <= VK_XBUTTON2) {
            int buttonOffset = key - VK_XBUTTON1;
            input.mi.dwFlags = MOUSEEVENTF_XUP | (buttonOffset << 2);
        }
    }

    // Simulate key release or mouse button release
    SendInput(1, &input, sizeof(INPUT));
} 