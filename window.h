#ifndef WINDOW_H
#define WINDOW_H

#include <fstream>

#include <iostream>

#include <set>
#include <string>
#include <windows.h>

class Screen {
    BITMAPINFOHEADER biHeader;
    BITMAPFILEHEADER bfHeader;
    BITMAPINFO bInfo;
    HBITMAP hBitmap;
    HDC hDC, hMemDC;
    char* bBits;

public:

    Screen() {
        hDC = GetDC(NULL);
        hMemDC = CreateCompatibleDC(hDC);

        bfHeader.bfType = 0x4D42;
        bfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        biHeader.biSize = sizeof(BITMAPINFOHEADER);
        biHeader.biBitCount = 24;
        biHeader.biCompression = BI_RGB;
        biHeader.biPlanes = 1;

        bInfo.bmiHeader = biHeader;

    }
    ~Screen() {
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hDC);
        DeleteObject(hBitmap);
    }


    char* screenshot(int x1, int y1, int x2, int y2) {
        int width{x2 - x1}, height{y2 - y1};

        bInfo.bmiHeader.biWidth = width;
        bInfo.bmiHeader.biHeight = height;  

        hBitmap = CreateDIBSection(hDC, &bInfo, DIB_RGB_COLORS, (VOID **)&bBits, NULL, 0);
        SelectObject(hMemDC, hBitmap);
        BitBlt(hMemDC, 0, 0, width, height, hDC, x1, y1, SRCCOPY);

        return bBits;

    }

    void save(int width, int height, char* bBits, const std::string& filename) {
        biHeader.biWidth = width;
        biHeader.biHeight = height;
        std::ofstream fout{filename, std::ios_base::out | std::ios_base::binary};
        fout.write((char*)&bfHeader, sizeof(BITMAPFILEHEADER));
        fout.write((char*)&biHeader, sizeof(BITMAPINFOHEADER));
        fout.write(bBits, (((24 * width + 31) & ~31) / 8) * height);
    }

};
    

class Window {

public:

    std::string activeWindow();
    //HBITMAP screenshot(int x1, int y1, int x2, int y2);
    void screenshot1(BITMAP* bitmap, int x1, int y1, int x2, int y2);
    COLORREF pixelColor(int x, int y, HBITMAP hBitmap = 0);
    std::set<int> getPIDs();
    int getPID(const std::string& name);
    bool isProcess(const std::string name);
    std::string getName(int PID);
    void kill(int PID);
    void kill(const std::string& name);
    /*
    std::wstring getClipboard();
    void setClipboard(const std::wstring& text);
    
    */
/*

    bool SaveBitmap(const std::string& filename)
    {
        BITMAPINFO bInfo;
        HGDIOBJ hTempBitmap;
        HBITMAP hBitmap;
        BITMAP bAllDesktops;
        HDC hDC, hMemDC;
        LONG lWidth, lHeight;
        BYTE *bBits = NULL;
        DWORD cbBits, dwWritten = 0;
        INT x = GetSystemMetrics(SM_XVIRTUALSCREEN);
        INT y = GetSystemMetrics(SM_YVIRTUALSCREEN);

        ZeroMemory(&bfHeader, sizeof(BITMAPFILEHEADER));
        ZeroMemory(&biHeader, sizeof(BITMAPINFOHEADER));
        ZeroMemory(&bInfo, sizeof(BITMAPINFO));
        ZeroMemory(&bAllDesktops, sizeof(BITMAP));

        hDC = GetDC(NULL);
        hTempBitmap = GetCurrentObject(hDC, OBJ_BITMAP);
        GetObjectW(hTempBitmap, sizeof(BITMAP), &bAllDesktops);

        lWidth = bAllDesktops.bmWidth;
        lHeight = bAllDesktops.bmHeight;

        DeleteObject(hTempBitmap);

        
        
        
        biHeader.biPlanes = 1;
        biHeader.biWidth = lWidth;
        biHeader.biHeight = lHeight;

        bInfo.bmiHeader = biHeader;

        cbBits = (((24 * lWidth + 31)&~31) / 8) * lHeight;

        hMemDC = CreateCompatibleDC(hDC);
        hBitmap = CreateDIBSection(hDC, &bInfo, DIB_RGB_COLORS, (VOID **)&bBits, NULL, 0);
        SelectObject(hMemDC, hBitmap);
        BitBlt(hMemDC, 0, 0, lWidth, lHeight, hDC, x, y, SRCCOPY);

        std::ofstream fout(filename, std::ios::out | std::ios::binary);
        fout.write((char*)&bfHeader, sizeof(bfHeader));
        fout.write((char*)&biHeader, sizeof(biHeader));
        fout.write((char*)bBits, cbBits);

        DeleteDC(hMemDC);
        ReleaseDC(NULL, hDC);
        DeleteObject(hBitmap);

        return TRUE;
    }*/

};

#endif