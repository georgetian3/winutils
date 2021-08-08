# winutils

Provides the functionality below for Windows operating system:

- Mouse and keyboard input simulation (`Input` class)
- Mouse and keyboard input hooking (`Hook` class)
- Retrieving and killing processes (`Window` class)
- Capturing screenshot, retrieving pixel colors, and saving as bitmap image (`Screen` class)
- Running command prompt commands and retrieving output (`exec` function)
- Getting current time, and stopwatch (`Timer` class)


## `Hook`

Allows users to hook keyboard and mouse input. One use case is the creation of custom hot keys, similar to the functionality of [AutoHotKey](https://github.com/AutoHotkey/AutoHotkey), whilst using C++ syntax.

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
