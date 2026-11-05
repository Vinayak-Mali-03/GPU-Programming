/**
 * @file Window.c
 * @author Vinayak B. Mali
 * @brief To Show different Window message's
 * @version 0.1
 * @date 2025-03-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */


// Win32 headers
#include <Windows.h>

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
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	
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
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// code
	switch (iMsg)
	{
		case WM_CREATE:
			MessageBox(hWnd, TEXT("This is the first message...!"), TEXT("WM_CREATE"), MB_OK | MB_ICONINFORMATION);
			break;

		case WM_SIZE:
			MessageBox(hWnd, TEXT("Window is resized...!"), TEXT("WM_SIZE"), MB_OK | MB_ICONINFORMATION);
			break;

		case WM_MOVE:
			MessageBox(hWnd, TEXT("Window is moved...!"), TEXT("WM_MOVE"), MB_OK | MB_ICONINFORMATION);
			break;

		case WM_KEYDOWN:
			switch(wParam)
			{
				case VK_ESCAPE:
					MessageBox(hWnd, TEXT("Escape key is pressed...!"), TEXT("WM_KEYDOWN"), MB_OK | MB_ICONINFORMATION);
					break;

				default:
					break;
			}
			break;

		case WM_CHAR:
			switch(wParam)
			{
				case 'F':
					MessageBox(hWnd, TEXT("F key is pressed...!"), TEXT("WM_CHAR"), MB_OK | MB_ICONINFORMATION);
					break;

				case 'f':
					MessageBox(hWnd, TEXT("f key is pressed...!"), TEXT("WM_CHAR"), MB_OK | MB_ICONINFORMATION);
					break;

				default:
					break;
			}
			break;

		case WM_LBUTTONDOWN:
			MessageBox(hWnd, TEXT("Left mouse button is clicked...!"), TEXT("WM_LBUTTONDOWN"), MB_OK | MB_ICONINFORMATION);
			break;

		case WM_CLOSE:
			MessageBox(hWnd, TEXT("Window is closing...!"), TEXT("WM_CLOSE"), MB_OK | MB_ICONINFORMATION);

			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			break;
	}

	return (DefWindowProc(hWnd, iMsg, wParam, lParam));
}

