#ifndef HOOK_H
#define HOOK_H

#include <vector>
#include <windows.h>


#define DEBUG

#ifdef DEBUG
    #include <iostream>
    #define dbg(x) x
#else 
    #define dbg(x)
#endif


struct Event {
    // if `key` = 0, then `direction` won't be populated
    // otherwise, `point` won't be populated
    int key{ 0 }; // virtual key code, `WM_MOUSEMOVE` for mouse movement, `WM_MOUSEWHEEL` for mouse wheel scroll
    bool direction{ false }; // false for key release and scroll up, true for key press and scroll down
    bool injected{ false }; // false for physical input, true for injected input
    POINT point{ 0, 0 }; // if `key` = 0, gives the position of cursor
};

namespace Hook {

    const int BLOCK{ 0 };
    const int PROPAGATE{ 1 };
    const int INTERRUPT{ 2 };

    namespace {
        
        bool states[256] = { 0 };
        int (*function)(const Event&){ nullptr };
        Event event;

        bool run_function() {
            if (event.key != WM_MOUSEMOVE && event.key != WM_MOUSEWHEEL) {
                states[event.key]  = event.direction;
                states[VK_SHIFT]   = states[VK_LSHIFT]   || states[VK_RSHIFT];
                states[VK_CONTROL] = states[VK_LCONTROL] || states[VK_RCONTROL];
                states[VK_MENU]    = states[VK_LMENU]    || states[VK_RMENU];
            }
            int action = function(event);
            if (action == INTERRUPT) {
                PostQuitMessage(0);
            }
            return action == PROPAGATE;
        }

        LRESULT CALLBACK keyboardHook(int nCode, WPARAM wParam, LPARAM lParam) {
            static PKBDLLHOOKSTRUCT keyboard{ (PKBDLLHOOKSTRUCT)lParam };
            event.key       =   keyboard->vkCode;
            event.direction = !(keyboard->flags & LLKHF_UP);
            event.injected  =   keyboard->flags & LLKHF_INJECTED;
            return run_function() ? CallNextHookEx(NULL, nCode, wParam, lParam) : 1;
        }
        LRESULT CALLBACK mouseHook(int nCode, WPARAM wParam, LPARAM lParam) {
            static PMSLLHOOKSTRUCT mouse{ (PMSLLHOOKSTRUCT)lParam };
            event.injected = mouse->flags;
            switch (wParam) {
                case WM_MOUSEMOVE:
                    event.key = WM_MOUSEMOVE;
                    event.point = mouse->pt;
                    break;
                case WM_MOUSEWHEEL:
                    event.key = WM_MOUSEWHEEL;
                    event.direction = mouse->mouseData == 0xff880000;
                    break;
                case WM_LBUTTONDOWN:
                    event.key = VK_LBUTTON;
                    event.direction = true;
                    break;
                case WM_LBUTTONUP:
                    event.key = VK_LBUTTON;
                    event.direction = false;
                    break;
                case WM_RBUTTONDOWN:
                    event.key = VK_RBUTTON;
                    event.direction = true;
                    break;
                case WM_RBUTTONUP:
                    event.key = VK_RBUTTON;
                    event.direction = false;
                    break;
                case WM_MBUTTONDOWN:
                    event.key = VK_MBUTTON;
                    event.direction = true;
                    break;
                case WM_MBUTTONUP:
                    event.key = VK_MBUTTON;
                    event.direction = false;
                    break;
                case WM_XBUTTONDOWN:
                case WM_XBUTTONUP:
                    event.key = mouse->mouseData >> 16 == 1 ? VK_XBUTTON1 : VK_XBUTTON2;
                    event.direction = wParam == WM_XBUTTONDOWN;
                    break;
                default:
                    dbg(std::cout << "default???\n");
            }
            run_function();
            return run_function() ? CallNextHookEx(NULL, nCode, wParam, lParam) : 1;
        }

    }

    void run(int (*new_function)(const Event&)) {

        dbg(std::cout << "Start run\n");

        HHOOK hKeyboardHook{ SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHook, 0, 0) };
        HHOOK hMouseHook   { SetWindowsHookEx(WH_MOUSE_LL   , mouseHook   , 0, 0) };

        function = new_function;
        GetKeyboardState((PBYTE)states);
        MSG msg;
        dbg(std::cout << "Entering loop\n");
        while (GetMessage(&msg, NULL, 0, 0));

        dbg(std::cout << "Unhooking\n");

        UnhookWindowsHookEx(hKeyboardHook);
        UnhookWindowsHookEx(hMouseHook);

        dbg(std::cout << "End run\n");

    }

    // type = false for physical state, type = true for virtual state
    bool state(int key, bool type = false) {
        if (key >= 0 && key <= 256) {
            if (type) {
                return GetKeyState(key) | 0x8000;
            }
            return states[key];
        }
        return false;
    }
    bool states_or(const std::vector<int>& keys, bool type = false) {
        for (int key : keys) {
            if (state(key, type)) {
                return true;
            }
        }
        return false;
    }
    bool states_and(const std::vector<int>& keys, bool type = false) {
        for (int key : keys) {
            if (!state(key, type)) {
                return false;
            }
        }
        return true;
    }

};

#endif