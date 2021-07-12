#ifndef HOOK_H
#define HOOK_H

#include <future>
#include <iostream>
#include <queue>
#include <vector>
#include <windows.h>
#include "input.h"

class Function {

private:

    bool (*trigger)(WPARAM, PKBDLLHOOKSTRUCT, PMSLLHOOKSTRUCT) { nullptr };
    void (*action)(WPARAM, PKBDLLHOOKSTRUCT, PMSLLHOOKSTRUCT) { nullptr };
    bool block{ false };
    bool slow{ false };

    std::future<void> thread;

    Function(
        void (*action)(WPARAM, PKBDLLHOOKSTRUCT, PMSLLHOOKSTRUCT),
        bool (*trigger)(WPARAM, PKBDLLHOOKSTRUCT, PMSLLHOOKSTRUCT) = nullptr,
        bool block = false,
        bool slow = false) :
        action(action), trigger(trigger), block(block), slow(slow) {}

    bool run(int event, PKBDLLHOOKSTRUCT keyboard, PMSLLHOOKSTRUCT mouse);

    friend class Hook;

};


class Hook {

    HHOOK hKeyboardHook, hMouseHook;
    PKBDLLHOOKSTRUCT keyboard;
    PMSLLHOOKSTRUCT mouse;
    std::vector<Function> functions;
    Input input = Input();
    MSG msg;

    void hook();
    void unhook();

public:

    Hook();
    ~Hook();
    void run();

    bool hookProc(bool type, WPARAM wParam, LPARAM lParam);

    void add(
        void (*action)(WPARAM, PKBDLLHOOKSTRUCT, PMSLLHOOKSTRUCT),
        bool (*trigger)(WPARAM, PKBDLLHOOKSTRUCT, PMSLLHOOKSTRUCT) = nullptr,
        bool block = false,
        bool thread = false
    );


};

#endif