#pragma once

#include <Windows.h>
#include <string>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

namespace Window
{
	// Getters
	unsigned int Width();
	unsigned int Height();
	float AspectRatio();
	HWND Handle();
	bool HasFocus();
	bool IsMinimized();

	// Window-related functions
	HRESULT Create(
		HINSTANCE appInstance,
		unsigned int width,
		unsigned int height,
		std::wstring titleBarText,
		bool statsInTitleBar,
		void (*resizeCallback)());
	void UpdateStats(float totalTime);
	void Quit();

	// Helper function for allocating a console window
	void CreateConsoleWindow(int bufferLines, int bufferColumns, int windowLines, int windowColumns);

	// OS-level message handling
	LRESULT ProcessMessage(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam);
}
