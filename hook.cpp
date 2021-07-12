#include <thread>
#include <vector>
#include <windows.h>
#include "hook.h"
#include "input.h"


bool Function::run(int event, PKBDLLHOOKSTRUCT keyboard, PMSLLHOOKSTRUCT mouse) {
    if (trigger != nullptr && !trigger(event, keyboard, mouse)) {
        return false;
    }
    if (slow) {
        if (!thread.valid() || thread.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            thread = std::async(std::launch::async, action, event, keyboard, mouse);
        }
    }
    else {
        action(event, keyboard, mouse);
    }
    return block;
}

Hook* hookInstance;

LRESULT CALLBACK keyboardHook(int nCode, WPARAM wParam, LPARAM lParam) {
    return hookInstance->hookProc(1, wParam, lParam) ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam);
}
LRESULT CALLBACK mouseHook(int nCode, WPARAM wParam, LPARAM lParam) {
    return hookInstance->hookProc(0, wParam, lParam) ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam);
}

bool Hook::hookProc(bool type, WPARAM wParam, LPARAM lParam) {

    if (Input::isActive({ VK_CONTROL, VK_SHIFT, VK_MENU, 'Q' })) {
        PostQuitMessage(0);
        return false;
    }

    keyboard = type ? (PKBDLLHOOKSTRUCT)lParam : nullptr;
    mouse = type ? nullptr : (PMSLLHOOKSTRUCT)lParam;

    bool block{ false };

    for (int i{ 0 }; i < functions.size(); ++i) {
        block = functions[i].run(wParam, keyboard, mouse) ? true : block;
    }

    return block;

}

Hook::Hook() {
    hookInstance = this;
}
void Hook::hook() {
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHook, 0, 0);
    hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseHook, 0, 0);
}
void Hook::unhook() {
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


void Hook::add(
    void (*action)(WPARAM, PKBDLLHOOKSTRUCT, PMSLLHOOKSTRUCT),
    bool (*trigger)(WPARAM, PKBDLLHOOKSTRUCT, PMSLLHOOKSTRUCT),
    bool block,
    bool thread) {
    functions.push_back(Function(action, trigger, block, thread));
}
