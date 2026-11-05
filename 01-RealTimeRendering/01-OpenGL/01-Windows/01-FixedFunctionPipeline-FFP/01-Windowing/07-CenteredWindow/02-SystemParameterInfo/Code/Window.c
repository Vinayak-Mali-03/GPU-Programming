/**
 * @file Window.c
 * @author Vinayak B. Mali
 * @brief Center the window using SystemParametersInfo() function
 * @version 0.1
 * @date 2025-03-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

// Win32 headers
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "Window.h"

// Macros
#define WIN_WIDTH	800
#define WIN_HEIGHT	600

// Global function declaration's
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Global variable declaration's

// Variable's releated with full screen 
BOOL gbFullScreen = FALSE;
HWND ghWnd = NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev;

// Variable's releated with FileI/O
char gszLogFileName[] = "Log.txt";
FILE* gpFile = NULL;

// Entry-Point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iShowCmd)
{
	// variable declaration's
	WNDCLASSEX wndclass;
	HWND hWnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("RTR6");
	RECT rect;

	// Code

	// Create Log file
	gpFile = fopen(gszLogFileName, "w");
	if (gpFile == NULL)
	{
		MessageBox(NULL, TEXT("Log file creation failed."), TEXT("File error"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Program started successfully.\n");
	}

	// Window class initialization
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
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

	if(!SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0))
	{
		fprintf(gpFile, "ERROR : SystemParametersInfo() function failed...!!!\n");
		exit(0);
	}

	// Create window
	hWnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("Vinayak B. Mali"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		(rect.right / 2) - (WIN_WIDTH / 2),
		(rect.bottom / 2) - (WIN_HEIGHT / 2),
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ghWnd = hWnd;

	// Show window
	ShowWindow(hWnd, iShowCmd);

	//  Paint background of the window
	UpdateWindow(hWnd);

	// Message loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Close the file
	if (gpFile)
	{
		fprintf(gpFile, "Program terminated successfully...!\n");
		fclose(gpFile);
		gpFile = NULL;
	}


	return ((int)msg.wParam);
}

// Callback function
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Local function declaration's
	void toggleFullScreen(void);

	// Local variable declaration's
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rc;
	TCHAR str[] = TEXT("Hello World !!!");

	// code
	switch (uMsg)
	{
	case WM_CREATE:
		ZeroMemory((void*)&wpPrev, sizeof(WINDOWPLACEMENT));
		wpPrev.length = sizeof(WINDOWPLACEMENT);
		break;

	case WM_CHAR:
		switch (wParam)
		{
		case 'F':
		case 'f':
			if (gbFullScreen == FALSE)
			{
				toggleFullScreen();

				gbFullScreen = TRUE;
			}
			else
			{
				toggleFullScreen();

				gbFullScreen = FALSE;
			}
			break;

		default:
			break;
		}
		break;

	case WM_PAINT:
		GetClientRect(hWnd, &rc);

		hdc = BeginPaint(hWnd, &ps);

		SetBkColor(hdc, RGB(0, 0, 0));
		SetTextColor(hdc, RGB(0, 255, 0));
		DrawText(hdc, str, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void toggleFullScreen(void)
{
	// Local variable declaration's
	MONITORINFO mi;

	// Code
	if (gbFullScreen == FALSE)
	{
		dwStyle = GetWindowLong(ghWnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			ZeroMemory((void*)&mi, sizeof(MONITORINFO));
			mi.cbSize = sizeof(MONITORINFO);

			if (GetWindowPlacement(ghWnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghWnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghWnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghWnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowPlacement(ghWnd, &wpPrev);
		SetWindowLong(ghWnd, GWL_STYLE, dwStyle | WS_EX_OVERLAPPEDWINDOW);
		SetWindowPos(ghWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
}



