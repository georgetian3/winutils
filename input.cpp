#include "input.h"
#include <cmath>
#include <string>


Input::Input() {

}

Input::~Input() {

}

int Input::send_inputs() {
    int inserted = SendInput(inputs.size(), inputs.data(), sizeof(INPUT));
    inputs.clear();
    return inserted;
}

/* bool Input::is_pressed(int key) {
    return GetAsyncKeyState(key) & 0x8000;
}

bool Input::is_pressed(const std::vector<int>& keys) {
    for (int key : keys) {
        if (!is_pressed(key)) {
            return false;
        }
    }
    return true;
}

bool Input::is_toggled(int key) {
    return GetKeyState(key) & 1;
}

POINT Input::position() {
    POINT point;
    GetCursorPos(&point);
    return point;
} */

void Input::move(int x, int y, bool relative, bool send) {
    inputs.emplace_back(INPUT());
    inputs.back().type = INPUT_MOUSE;
    inputs.back().mi.dx = x;
    inputs.back().mi.dy = y;
    inputs.back().mi.dwFlags = MOUSEEVENTF_MOVE;
    if (!relative) {
        inputs.back().mi.dx *= x_scale;
        inputs.back().mi.dy *= y_scale;
        inputs.back().mi.dwFlags |= MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
    }
    if (send) {
        send_inputs();
    }
}

void Input::scroll(int lines, bool send) {
    inputs.emplace_back(INPUT());
    inputs.back().type = INPUT_MOUSE;
    inputs.back().mi.mouseData = lines;
    inputs.back().mi.dwFlags = MOUSEEVENTF_WHEEL;
    if (send) {
        send_inputs();
    }
}

const int mouseFlags[2][5]{
    {0, MOUSEEVENTF_LEFTUP, MOUSEEVENTF_RIGHTUP, 0, MOUSEEVENTF_MIDDLEUP},
    {0, MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_RIGHTDOWN, 0, MOUSEEVENTF_MIDDLEDOWN}
};

void Input::press(int key, int direction, bool send) {
    if (direction == BOTH) {
        press(key, DOWN, false);
        press(key, UP, send);
        return;
    }
    inputs.emplace_back(INPUT());
    if (key >= 1 && key <= 6 && key != 3) {
        inputs.back().type = INPUT_MOUSE;
        if (key < 5) {
            inputs.back().mi.dwFlags = mouseFlags[direction][key];
        }
        else {
            inputs.back().mi.dwFlags = (direction == DOWN) ? MOUSEEVENTF_XDOWN : MOUSEEVENTF_XUP;
            inputs.back().mi.mouseData = (key == VK_XBUTTON1) ? XBUTTON1 : XBUTTON2;
        }
    }
    else {
        inputs.back().type = INPUT_KEYBOARD;
        inputs.back().ki.wScan = MapVirtualKey(key, MAPVK_VK_TO_VSC);
        inputs.back().ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP * (direction == UP);
    }
    if (send) {
        send_inputs();
    }
}

void Input::combo(const std::vector<int>& keys, bool send) {
    for (int key : keys) {
        press(key, DOWN, false);
    }
    for (int key : keys) {
        press(key, UP, false);
    }
    if (send) {
        send_inputs();
    }
}

void Input::type(const std::string& str, bool send) {
    for (const char& c : str) {
        inputs.emplace_back(INPUT());
        inputs.back().type = INPUT_KEYBOARD;
        inputs.back().ki.wScan = c;
        inputs.back().ki.dwFlags = KEYEVENTF_UNICODE;
        inputs.emplace_back(INPUT());
        inputs.back().type = INPUT_KEYBOARD;
        inputs.back().ki.wScan = c;
        inputs.back().ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
    }
    if (send) {
        send_inputs();
    }
}

void Input::type(const std::wstring& str, bool send) {
    for (const wchar_t& c : str) {
        inputs.back().type = INPUT_KEYBOARD;
        inputs.back().ki.wScan = c;
        inputs.back().ki.dwFlags = KEYEVENTF_UNICODE;
        inputs.emplace_back(INPUT());
        inputs.back().type = INPUT_KEYBOARD;
        inputs.back().ki.wScan = c;
        inputs.back().ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
        inputs.emplace_back(INPUT());
    }
    if (send) {
        send_inputs();
    }
}