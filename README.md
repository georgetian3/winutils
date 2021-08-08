# winutils

Provides the functionality below for Windows operating system:

- Mouse and keyboard input simulation (`Input` class)
- Mouse and keyboard input hooking (`Hook` class)
- Retrieving and killing processes (`Window` class)
- Capturing screenshot, retrieving pixel colors, and saving as bitmap image (`Screen` class)
- Running command prompt commands and retrieving output (`exec` function)
- Getting current time, and stopwatch (`Timer` class)

## `Input`

Allows users to emulate mouse and keyboard input.





The following constants relate to the direction of key presses:
 - `DOWN`: key is pressed down
 - `UP`: key is released up
 - `BOTH`: the key is pressed then immediately released

Contains the following functions:

 - `void move(int x, int y, bool relative = false, bool send = true)`: emulates mouse movement. If `relative` is false, moves mouse pointer to the pixel at (`x`, `y`), otherwise moves mouse pointer `x` pixels right and `y` pixels down from the current pointer position.
 - `void scroll(int lines, bool send = true)`: emulates mouse wheel scrolling. Negative lines indicates scrolling up and vice versa.
 - `void press(int key, int direction = BOTH, bool send = true)`: emulates a mouse button click / keyboard key press. `key` is the [virtual code](https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes) of the key, `direction` takes a constant explained above.
 - `void combo(const std::vector<int>& keys, bool send = true)`: emulates a key combo. Given a vector of virtual key codes, `press(key, DOWN, false)` is called for each key in order, then `press(key, UP, false)` is called for each key in reverse order, then all events are sent.
 - `void type(const std::string& str, bool send = true)`: enters a string instead of a series of keyboard inputs.
 - `void type(const std::wstring& str, bool send = true)`: same as above, but for strings containing wide characters.

 All the functions above have the parameter `send`. This is because all functions above first append the input event to an array, before using Window's `SendInput` API to inject the events. If it is intended for multiple events to be emulated successively, then it is recommended to give all but the last function call's parameter `send` the value of `false`. Alternatively, call `send_inputs()` when you wish for all prepared events to be injected. `send_inputs()` returns the number of events that were actually injected.

 The constructor takes an `int` which defines the length of the array that holds input events. Error handling for when one prepares more events than the array can hold is currently not implemented.

## `Hook`

Allows users to hook keyboard and mouse input. One use case is the creation of custom hot keys, similar to the functionality of [AutoHotKey](https://github.com/AutoHotkey/AutoHotkey), whilst using C++ syntax.

Users can add functions to the `Hook` instance for it to be executed when certain events occur. Mouse and keyboard events are translated by `Hook` into `Event` structures, which contain the following variables:

 - `key`: the [virtual key code](https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes) of the button or key that was pressed or released. The two events that are not strictly virtual key codes are `WM_MOUSEMOVE` which indicates the mouse pointer's position has changed, and `WM_MOUSEWHEEL` which indicates the mouse wheel has been scrolled.
 - `direction`: `true` if the key was pressed, `false` if the key was released. When `key` has the value `WM_MOUSEWHEEL`, `false` indicates the scroll wheel was moved up away from the user, and `true` otherwise. Not applicable if `key` has value `WM_MOUSEMOVE`.
 - `injected`: `true` if the input event was injected by this or another application, `false` otherwise.
 - `point`: contains the x and y coordinates of the mouse pointer. Only applicable if `key` has the value `WM_MOUSEMOVE`

 When adding to the hook, the user must provide an `action` function that returns a `bool`, and optionally a `trigger` function which returns an `int`. Both functions must take an `Event` structure as their only paramter.

 If the function can be executed quickly (such as injecting a few keystrokes), only providing `action` is sufficient. It must return whether or not the event should be blocked from further propagating through Window's message queue.

 Otherwise, if `action` is slow, a `trigger` function must be provided, the return value of which decides whether or not to execute the main `action function, as well as whether or not to block the event:
  - `0`: `action` is not executed, event is not blocked
  - `1`: `action` is executed in a separate thread, event is not blocked
  - `2`: `action` is executed in a separate thread, event is blocked

Currently, each slow `action` must be completed before it can be executed again.


Below is an example of using `Hook` and `Input` together to remap the side mouse buttons to `Ctrl-C` and `Ctrl-V`

    #include "hook.h"
    #include "input.h"
    
    Input input;
    Hook hook; // currently only one instance of `Hook` can be created

    // define an interrupt function which is evaluated before all other functions
    // when it returns true, all hooks are removed
    // the below function will ensure that when Ctrl+Shift+Alt+Q is pressed, hooks will be removed
    bool interrupt(Event event) {
        if (event.key == 'Q' && Input::isActive({ VK_CONTROL, VK_SHIFT, VK_MENU })) {
            std::cout << "INTERRUPT\n";
            return true;
        }
        return false;
    }
    
    // When side button 1 is pressed down, send a Ctrl+C key combo
    // return true to block the side button 1 keypress from Windows
    bool copy_a(Event event) {
        if (event.key == VK_XBUTTON1) {
            if (event.direction) {
                std::cout << "Copying\n";
                input.combo({ VK_CONTROL, 'C' });
            }
            return true;
        }
        return false;
    }
    // When side button 1 is pressed down, send a Ctrl+V key combo
    bool paste_a(Event event) {
        if (event.key == VK_XBUTTON2) {
            if (event.direction) {
                std::cout << "Pasting\n";
                input.combo({ VK_CONTROL, 'V' });
            }
            return true;
        }
        return false;
    }
    
    int main() {
        
        // register the interrupt function
        hook.setInterrupt(interrupt);

        // register the copy and paste functions
        hook.add(copy_a);
        hook.add(paste_a);

        // start hooking input
        hook.run();

    }
