/**
 * @file Window.c
 * @author Vinayak B. Mali
 * @brief Add icon to the file
 * @version 0.1
 * @date 2025-03-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

// Win32 headers
#include <Windows.h>
#include "Window.h"

// Global function declaration's
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Entry-Point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iShowCmd)
{
	// variable declaration's
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("RTR6");

	// code
	
	// Window class initialization
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	
	//  Registeration of window class
	RegisterClassEx(&wndclass);

	// Create window
	hwnd = CreateWindow(szAppName,
						TEXT("Vinayak B. Mali"),
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						NULL,
						NULL,
						hInstance,
						NULL
					);

	// Show window
	ShowWindow(hwnd, iShowCmd);

	//  Paint background of the window
	UpdateWindow(hwnd);

	// Message loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return ((int)msg.wParam);
}

// Callback function
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// code
	switch (uMsg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			break;
	}

	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

