#include "input.h"
#include <cmath>
#include <string>


Input::Input(int max_size) {
    max_size_ = max_size;
    inputs = new INPUT[max_size_];
}

Input::~Input() {
    delete[] inputs;
}

int Input::send_inputs() {
    int inserted = SendInput(cInputs, inputs, sizeof(INPUT));
    cInputs = 0;
    return inserted;
}

bool Input::is_pressed(int key) {
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
}

void Input::move(int x, int y, bool relative, bool send) {
    inputs[cInputs].type = INPUT_MOUSE;
    inputs[cInputs].mi.dx = x;
    inputs[cInputs].mi.dy = y;
    inputs[cInputs].mi.dwFlags = MOUSEEVENTF_MOVE;
    if (!relative) {
        inputs[cInputs].mi.dx *= x_scale;
        inputs[cInputs].mi.dy *= y_scale;
        inputs[cInputs].mi.dwFlags |= MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
    }
    ++cInputs;
    if (send) {
        send_inputs();
    }
}

void Input::scroll(int lines, bool send) {
    inputs[cInputs].type = INPUT_MOUSE;
    inputs[cInputs].mi.mouseData = lines;
    inputs[cInputs].mi.dwFlags = MOUSEEVENTF_WHEEL;
    ++cInputs;
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
    if (key >= 1 && key <= 6 && key != 3) {
        inputs[cInputs].type = INPUT_MOUSE;
        if (key < 5) {
            inputs[cInputs].mi.dwFlags = mouseFlags[direction][key];
        }
        else {
            inputs[cInputs].mi.dwFlags = (direction == DOWN) ? MOUSEEVENTF_XDOWN : MOUSEEVENTF_XUP;
            inputs[cInputs].mi.mouseData = (key == VK_XBUTTON1) ? XBUTTON1 : XBUTTON2;
        }
    }
    else {
        inputs[cInputs].type = INPUT_KEYBOARD;
        inputs[cInputs].ki.wScan = MapVirtualKey(key, MAPVK_VK_TO_VSC);
        inputs[cInputs].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP * (direction == UP);
    }
    ++cInputs;
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
    ++cInputs;
    for (const char& c : str) {
        inputs[cInputs].type = INPUT_KEYBOARD;
        inputs[cInputs].ki.wScan = c;
        inputs[cInputs].ki.dwFlags = KEYEVENTF_UNICODE;
        ++cInputs;
        inputs[cInputs].type = INPUT_KEYBOARD;
        inputs[cInputs].ki.wScan = c;
        inputs[cInputs].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
        ++cInputs;
    }
    if (send) {
        send_inputs();
    }
}

void Input::type(const std::wstring& str, bool send) {
    ++cInputs;
    for (const wchar_t& c : str) {
        inputs[cInputs].type = INPUT_KEYBOARD;
        inputs[cInputs].ki.wScan = c;
        inputs[cInputs].ki.dwFlags = KEYEVENTF_UNICODE;
        ++cInputs;
        inputs[cInputs].type = INPUT_KEYBOARD;
        inputs[cInputs].ki.wScan = c;
        inputs[cInputs].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
        ++cInputs;
    }
    if (send) {
        send_inputs();
    }
}