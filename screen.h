#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>


class Color {

    int red;
    int green;
    int blue;

public:

    Color(int r = 0, int g = 0, int b = 0) {
        red = r;
        green = g;
        blue = b;
    }

    void setR(int r) {
        red = r;
    }

    void setG(int g) {
        green = g;
    }

    void setB(int b) {
        blue = b;
    }

    void setRGB(int rgb) {
        red = (rgb >> 16) & 0xFF;
        green = (rgb >> 8) & 0xFF;
        blue = rgb & 0xFF;
    }

    void setBGR(int bgr) {
        blue = (bgr >> 16) & 0xFF;
        green = (bgr >> 8) & 0xFF;
        red = bgr & 0xFF;
    }

    int getR() {
        return red;
    }

    int getG() {
        return green;
    }

    int getB() {
        return blue;
    }

    int getRGB() {
        return (red << 16) | (green << 8) | blue;
    }

    int getBGR() {
        return (blue << 16) | (green << 8) | red;
    }

    friend bool operator== (const Color& a, const Color& b) {
        return a.red == b.red && a.green == b.green && a.blue == b.blue;
    }

    friend bool operator!= (const Color& a, const Color& b) {
        return !(a == b);
    }

    void printRGB() {
        std::cout << '(' << red << ',' << green << ',' << blue << ')' << '\n';
    }

};


class Screen {
    BITMAPFILEHEADER bfHeader;
    BITMAPINFO bInfo;
    HBITMAP hBitmap{ NULL };
    HDC hDC, hMemDC;
    char* bBits{ nullptr };
    int width{ 0 }, height{ 0 };

public:

    Screen() {
        hDC = GetDC(NULL);
        hMemDC = CreateCompatibleDC(hDC);

        bfHeader.bfType = 0x4D42;
        bfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

        bInfo.bmiHeader.biBitCount = 24;
        bInfo.bmiHeader.biCompression = BI_RGB;
        bInfo.bmiHeader.biPlanes = 1;
        bInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

    }
    
    ~Screen() {
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hDC);
        if (hBitmap) {
            DeleteObject(hBitmap);
        }
    }


    char* screenshot(int x1, int y1, int x2, int y2) {

        width = x2 - x1;
        height = y2 - y1;

        bInfo.bmiHeader.biWidth = width;
        bInfo.bmiHeader.biHeight = height;

        if (hBitmap) {
            DeleteObject(hBitmap);
        }
        
        hBitmap = CreateDIBSection(hDC, &bInfo, DIB_RGB_COLORS, (VOID**)&bBits, NULL, 0);
        SelectObject(hMemDC, hBitmap);
        BitBlt(hMemDC, 0, 0, width, height, hDC, x1, y1, SRCCOPY);

        return bBits;

    }

    Color getColor(int x, int y) {
        if (!bBits) {
            return Color();
        }
        int pos = (width * 3 + 3) / 4 * 4 * (height - y - 1) + x * 3;
        return Color(
            (uint8_t)bBits[pos + 2],
            (uint8_t)bBits[pos + 1],
            (uint8_t)bBits[pos]
        );
    }

    void save(const std::string& filename) {
        std::ofstream fout{filename, std::ios_base::out | std::ios_base::binary};
        fout.write((char*)&bfHeader, sizeof(BITMAPFILEHEADER));
        fout.write((char*)&bInfo.bmiHeader, sizeof(BITMAPINFOHEADER));
        fout.write(bBits, (width * 3 + 3) / 4 * 4 * height * 3);
    }

};