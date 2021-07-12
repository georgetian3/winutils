#include "screen.h"
#include "timer.h"
#include <iostream>
using namespace std;

int main() {

	while (1) {
		cout << Timer::iso_time() << '\n';
		Sleep(1);
	}
	return 0;

	Screen s;
	int x1{ 0 }, x2{ 2560 }, y1{ 0 }, y2{ 1440 };

	char* bits;
	bits = s.screenshot(x1, y1, x2, y2);

	Color c = s.getColor(1600, 1200);
	std::cout << "Color: " << (int)c.getR() << "," << (int)c.getG() << "," << (int)c.getB();
	s.save("test.bmp");
}