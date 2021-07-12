#include "window.h"
#include <iostream>
#include <psapi.h>
#include <windows.h>

std::string Window::activeWindow() {
	char title[256];
	GetWindowTextA(GetForegroundWindow(), title, sizeof(title));
	return title;
}

std::set<int> Window::getPIDs() {
	DWORD pids[1024]{0};
	DWORD needed;
	if (!EnumProcesses(pids, sizeof(pids), &needed)) {
		std::cout << "EmunProcesses Error";
	}
	return std::set<int>(pids, pids + needed / sizeof(DWORD));
}

int Window::getPID(const std::string& name) {
	std::set<int> pids = getPIDs();
	for (int pid: pids) {
		if (getName(pid) == name) {
			return pid;
		}
	}
	return -1;
}

bool Window::isProcess(const std::string name) {
	return getPID(name) != -1;
}

std::string Window::getName(int PID) {
	char name[256] = "";
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, PID);
	GetModuleBaseName(hProcess, NULL, (LPWSTR)name, sizeof(name));
	CloseHandle(hProcess);
	return name;
}

void Window::kill(int PID) {
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, false, PID);
	TerminateProcess(hProcess, 0);
	CloseHandle(hProcess);
}

void Window::kill(const std::string& name) {
	kill(getPID(name));
}
/*
std::wstring Window::getClipboard() {
	if (OpenClipboard(NULL)) {
		HANDLE h = GetClipboardData(CF_UNICODETEXT);
		std::wstring text = (wchar_t*)GlobalLock(h);
		GlobalUnlock(h);
		CloseClipboard();
		return text;
	}
	return L"<OpenClipboard FAILED>";
}

void Window::setClipboard(const std::wstring& text) {
	if (OpenClipboard(NULL)) {
		EmptyClipboard();
		HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, text.size() + 1);
		wchar_t* buffer = (wchar_t*)GlobalLock(clipbuffer);
		wcscpy(buffer, text.c_str());
		SetClipboardData(CF_UNICODETEXT, clipbuffer);
		CloseClipboard();
	}
}
*/