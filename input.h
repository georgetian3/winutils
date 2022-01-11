#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <vector>
#include <windows.h>


// inputs are not inserted but are stored in `inputs` until either a function is called with the parameter
// `send` as true, or `send_inputs` is called

class Input {

private:

	const double x_scale{ 65535.0 / (GetSystemMetrics(SM_CXVIRTUALSCREEN) - 1) };
	const double y_scale{ 65535.0 / (GetSystemMetrics(SM_CYVIRTUALSCREEN) - 1) };

	// stores events to be inputted
	std::vector<INPUT> inputs;

	bool max_limits();

public:

	int send_inputs();

	// constants defining whether a button/key is pressed DOWN, released UP, or DOWNUP
	static const int UP{ 0 };
	static const int DOWN{ 1 };
	static const int DOWNUP{ 2 };

/* 	Input();
	~Input(); */

/* 	// returns whether a key, given its virtual key code, is currently pressed down
	static bool is_pressed(int key);
	static bool is_pressed(const std::vector<int>& keys);

	// returns if a key such as caps lock, scroll lock is toggled on
	// if key is not a togglable key, return value is undefined
	static bool is_toggled(int key);

	// returns the current position of the mouse
	static POINT position(); */

	// moves mouse to specified position
	bool move(int x, int y, bool relative = false, bool send = true);

	// scrolls the mouse wheel up (lines < 0) or down (lines > 0)
	bool scroll(int lines, bool send = true);

	// presses a key given its virtual code in the specified direction
	bool press(int key, int direction = DOWNUP, bool send = true);

	// given a vector of keys, presses each down in order, then releases each in reverse order
	bool combo(const std::vector<int>& keys, bool send = true);

	// types a string of normal characters
	bool type(const std::string& str, bool send = true);

	// types a string of wide characters
	bool type(const std::wstring& str, bool send = true);

};

#endif
