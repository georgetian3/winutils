#ifndef HOOK_H
#define HOOK_H

#include <future>
#include <iostream>
#include <queue>
#include <vector>
#include <windows.h>
#include "input.h"


struct Event {
    // if key is 0, then direction won't be populated
    // otherwise, point won't be populated
    int key; // virtual key code, `WM_MOUSEMOVE` for mouse movement, `WM_MOUSEWHEEL` for mouse wheel scroll
    bool direction; // false for key release / scroll up, true for key press / scroll down
    bool injected; // false for physical input, true for injected input
    POINT point; // if key is 0, gives the position of cursor
};


// every `Function` must have an `action` function that executes the actual macro, combo etc.
// `action` returns a bool that indicated whether or not the `Event` should continue through message queue
// a `trigger` function should be defined if the action function takes a relatively long time to complete,
// so that `action` function can be run in a separate thread
// if `trigger` is defined, the return value of `action` is ignored
// instead, `trigger`'s return value will indicate if the event should be blocked
// 0 indicates the action function should not be run
// 1 indicates the action function should be run, but the event will be passed on
// 2 indicates the action function should be run, and the event will be blocked

class Function {

private:

    bool (*action)(Event&) { nullptr };
    int (*trigger)(Event&) { nullptr };

    std::future<bool> thread;

    Function(bool (*action)(Event&), int (*trigger)(Event&) = nullptr) :
        action(action), trigger(trigger) {}

    bool run(Event&);

    friend class Hook;

};


class Hook {

    HHOOK hKeyboardHook{ nullptr };
    HHOOK hMouseHook{ nullptr };
    PKBDLLHOOKSTRUCT keyboard{ nullptr };
    PMSLLHOOKSTRUCT mouse{ nullptr };
    std::vector<Function> functions;
    bool (*interrupt)(Event&) { nullptr };
    Input input;
    Event event;
    bool states[256] = { 0 };
    MSG msg{ nullptr };

    void hook();
    void unhook();

public:

    Hook();
    ~Hook();
    void run();

    bool hookProc(bool type, WPARAM wParam, LPARAM lParam);

    void add(bool (*action)(Event&), int (*trigger)(Event&) = nullptr);
    void set_interrupt(bool (*function)(Event&));
    void remove(bool (*action)(Event&), int (*trigger)(Event&) = nullptr);
    bool state(int key);
    bool state(const std::vector<int>& keys);

};

#endif