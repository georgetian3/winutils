#include "hook.h"
#include "input.h"

#include <thread>
#include <vector>
#include <windows.h>



bool Function::run(Event& event) {
    if (trigger) {
        int trigger_return = trigger(event);
        if (trigger_return) {
            if (!thread.valid() || thread.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                thread = std::async(std::launch::async, action, std::ref(event));
            }
            return trigger_return == 2;
        }
        return false;
    }
    else {
        return action(event);
    }
}

Hook* hookInstance;

LRESULT CALLBACK keyboardHook(int nCode, WPARAM wParam, LPARAM lParam) {
    return hookInstance->hookProc(1, wParam, lParam) ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam);
}
LRESULT CALLBACK mouseHook(int nCode, WPARAM wParam, LPARAM lParam) {
    return hookInstance->hookProc(0, wParam, lParam) ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam);
}

bool Hook::hookProc(bool device, WPARAM wParam, LPARAM lParam) {

    // constructing Event
    if (device) { // keyboard input
        keyboard = (PKBDLLHOOKSTRUCT)lParam;
        event.key = keyboard->vkCode;
        event.direction = !(keyboard->flags & LLKHF_UP);
        event.injected = keyboard->flags & LLKHF_INJECTED;
    }
    else { // mouse input
        mouse = (PMSLLHOOKSTRUCT)lParam;
        if (wParam == WM_MOUSEMOVE) {
            event.key = WM_MOUSEMOVE;
            event.point = mouse->pt;
        }
        else if (wParam == WM_MOUSEWHEEL) {
            event.key = WM_MOUSEWHEEL;
            event.direction = mouse->mouseData == 0xff880000;
        }
        else if (wParam == WM_LBUTTONDOWN) {
            event.key = VK_LBUTTON;
            event.direction = true;
        }
        else if (wParam == WM_LBUTTONUP) {
            event.key = VK_LBUTTON;
            event.direction = false;
        }
        else if (wParam == WM_RBUTTONDOWN) {
            event.key = VK_RBUTTON;
            event.direction = true;
        }
        else if (wParam == WM_RBUTTONUP) {
            event.key = VK_RBUTTON;
            event.direction = false;
        }
        else if (wParam == WM_MBUTTONDOWN) {
            event.key = VK_MBUTTON;
            event.direction = true;
        }
        else if (wParam == WM_MBUTTONUP) {
            event.key = VK_MBUTTON;
            event.direction = false;
        }
        else if (wParam == WM_XBUTTONDOWN || wParam == WM_XBUTTONUP) {
            if (mouse->mouseData >> 16 == 1) {
                event.key = VK_XBUTTON1;
            }
            else {
                event.key = VK_XBUTTON2;
            }
            event.direction = wParam == WM_XBUTTONDOWN ? true : false;
        }
        event.injected = mouse->flags;
    }

    if (event.key != WM_MOUSEMOVE && event.key != WM_MOUSEWHEEL) {
        states[event.key] = event.direction;
        states[VK_SHIFT] = states[VK_LSHIFT] || states[VK_RSHIFT];
        states[VK_CONTROL] = states[VK_LCONTROL] || states[VK_RCONTROL];
        states[VK_MENU] = states[VK_LMENU] || states[VK_RMENU];
    }


    if (interrupt && interrupt(event)) {
        PostQuitMessage(0);
        return false;
    }

    for (int i{ 0 }; i < functions.size(); ++i) {
        if (functions[i].run(event)) {
            return true;
        }
    }

    return false;

}

Hook::Hook() {
    hookInstance = this;
    
    for (int i = 0; i < 256; i++) {
        states[i] = Input::is_pressed(i);
    }

}
void Hook::hook() {
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHook, 0, 0);
    hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseHook, 0, 0);
}
void Hook::unhook() {
    UnhookWindowsHookEx(hKeyboardHook);
    UnhookWindowsHookEx(hMouseHook);
}
Hook::~Hook() {
    unhook();
}

void Hook::run() {
    hook();
    while (GetMessage(&msg, NULL, 0, 0)) {
        DispatchMessage(&msg);
    }
}


void Hook::add(bool (*action)(Event&), int (*trigger)(Event&)) {
    functions.push_back(Function(action, trigger));
}

void Hook::set_interrupt(bool (*function)(Event&)) {
    interrupt = function;
}

void Hook::remove(bool (*action)(Event&), int (*trigger)(Event&)) {
    for (int i = static_cast<int>(functions.size()) - 1; i >= 0; i--) {
        if (functions[i].action == action && functions[i].trigger == trigger) {
            functions.erase(functions.begin() + i);
        }
    }
}

bool Hook::state(int key) {
    if (key >= 0 && key <= 256) {
        return states[key];
    }
    return false;
}

bool Hook::state(const std::vector<int>& keys) {
    for (int key : keys) {
        if (!state(key)) {
            return false;
        }
    }
    return true;
}