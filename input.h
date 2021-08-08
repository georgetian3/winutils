#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <windows.h>
#include <vector>

// if the parameter `injected` is true, then the `dwExtraInfo` member of keyboard or mouse struct
// will have the value 1, which is useful for checking if an input was created by the user or by Input

// inputs are not inserted but are stored in `inputs` until either a function is called with the parameter
// `send` as true, or `sendInputs` is called

class Input {

private:

	const double x_scale{ 65535.0 / (GetSystemMetrics(SM_CXVIRTUALSCREEN) - 1) };
	const double y_scale{ 65535.0 / (GetSystemMetrics(SM_CYVIRTUALSCREEN) - 1) };

	// array storing events to be inputted
	INPUT* inputs;
	// counter for the number of events in `inputs`
	int cInputs{ 0 };

public:

	int sendInputs();

	// constants defining whether a button/key is pressed DOWN, released UP, or BOTH
	static const int UP{ 0 };
	static const int DOWN{ 1 };
	static const int BOTH{ 2 };
	static const bool ANY{ false };
	static const bool ALL{ true };

	// maxSize defines the size of `inputs`, which limits how many inputs can be inserted at once
	Input(int maxSize = 65536);
	~Input();

	// returns whether a key, given its virtual key code, is currently pressed down
	static bool isActive(int key);
	static bool isActive(const std::vector<int>& keys);

	// returns if a key such as caps lock, scroll lock is toggled on
	// if key is not a togglable key, return value is undefined
	static bool isToggled(int key);

	// returns the current position of the mouse
	static POINT position();

	// moves mouse to specified position
	void move(int x, int y, bool relative = false, bool injected = false, bool send = true);

	// scrolls the mouse wheel up (lines < 0) or down (lines > 0)
	void scroll(int lines, bool injected = false, bool send = true);

	// presses a key given its virtual code in the specified direction
	void press(int key, int direction = BOTH, bool injected = false, bool send = true);

	// given a vector of keys, presses each down in order, then releases each in reverse order
	void combo(const std::vector<int>& keys, bool injected = false, bool send = true);

	// types a string of normal characters
	void type(const std::string& str, bool injected = false, bool send = true);

	// types a string of wide characters
	void type(const std::wstring& str, bool injected = false, bool send = true);

};

#endif
