// Win32 / Standard header file's
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES	1
#include <math.h>

// OpgnGl releated header file's
#include <gl/GL.h>
#include <gl/GLU.h>

// Custom header file's
#include "OGL.h"

// OpglGl releated librarie's
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "GLU32.lib")

// Macros
#define WIN_WIDTH	800
#define WIN_HEIGHT	600

// Global function declaration's
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Global variable declaration's

// Active window releated variable's
BOOL gbActiveWindow = FALSE;

// Exit keypress releated
BOOL gbEscapeKeyIsPressed = FALSE;

// Variable's releated with full screen 
BOOL gbFullScreen = FALSE;
HWND ghWnd = NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev;

// Variable's releated with FileI/O
char gszLogFileName[] = "Log.txt";
FILE* gpFile = NULL;

// OpenGL releated global variable's
HDC ghdc = NULL;
HGLRC ghrc = NULL;

// Rotation angle's
GLfloat angleCube = 0.0f;

float identityMatrix[16];
float translationMatrix[16];
float scaleMatrix[16];
float rotationMatrixX[16];
float rotationMatrixY[16];
float rotationMatrixZ[16];

// Entry-Point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iShowCmd)
{
	// Local function declaration's
	int initialize(void);
	void display(void);
	void update(void);
	void uninitialize(void);

	// variable declaration's
	WNDCLASSEX wndclass;
	HWND hWnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("RTR6");
	BOOL bDone = FALSE;

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

	// Create window
	hWnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("Vinayak B. Mali"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		(GetSystemMetrics(SM_CXSCREEN) / 2) - (WIN_WIDTH / 2),
		(GetSystemMetrics(SM_CYSCREEN) / 2) - (WIN_HEIGHT / 2),
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

	// Initialize
	int result = initialize();
	if (result != 0)
	{
		fprintf(gpFile, "Initialize() failed...!\n");
		DestroyWindow(hWnd);
		hWnd = NULL;
	}
	else
	{
		fprintf(gpFile, "Initialize() completed successfully...!\n\n");
	}

	// Set this window as forground and active window
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	// Game loop
	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == TRUE)
			{
				if (gbEscapeKeyIsPressed == TRUE)
				{
					bDone = TRUE;
				}

				// Render
				display();

				// Update
				update();
			}
		}
	}

	// Uninitialize
	uninitialize();

	return ((int)msg.wParam);
}

// Callback function
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Local function declaration's
	void resize(int, int);
	void uninitialize(void);
	void toggleFullScreen(void);

	// code
	switch (uMsg)
	{
	case WM_CREATE:
		ZeroMemory((void*)&wpPrev, sizeof(WINDOWPLACEMENT));
		wpPrev.length = sizeof(WINDOWPLACEMENT);
		break;

	case WM_SETFOCUS:
		gbActiveWindow = TRUE;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = FALSE;
		break;

	case WM_ERASEBKGND:
		return(0);

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			gbEscapeKeyIsPressed = TRUE;
			break;
			
		default:
			break;
		}
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

	case WM_CLOSE:
		uninitialize();
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

int initialize(void)
{
	// Local variable declaration's
	void printGLInfo(void);
	void resize(int, int);

	// Variable declaration's
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	// Code

	// PixelFormatDescriptor initializaiton
	ZeroMemory((void*)&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;
	
	// GetDC
	ghdc = GetDC(ghWnd);
	if (ghdc == NULL)
	{
		fprintf(gpFile, "ERROR : GetDC() Function Function Failed...!!!\n");
		return(-1);
	}

	// Get matching pixel format index using hdc and pfd
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		fprintf(gpFile, "ERROR : ChoosePixelFormat() Failed...!!!\n");
		return(-2);
	}
		
	// Select the pixel formats of found index
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		fprintf(gpFile, "ERROR : SetPixelFormat() Function Failed...!!!\n");
		return(-3);
	}

	// Create rendering context using hdc, pfd and choosen pixelformatindex
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(gpFile, "ERROR : wglCreateContext() Function Failed...!!!\n");
		return (-4);
	}

	// Make this rendering context as current context
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFile, "ERROR : wglMakeCurrent() Function Failed...!!!\n");
		return(-5);
	}

	// Print OpenGL info
	printGLInfo();

	// From here onwords OpelGL code start

	// Depth releated code
	glShadeModel(GL_SMOOTH);

	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Tell OpenGl to choose the color to clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Define matrices
	//Identity matrix
	identityMatrix[0] = 1.0f;
	identityMatrix[1] = 0.0f;
	identityMatrix[2] = 0.0f;
	identityMatrix[3] = 0.0f;
	identityMatrix[4] = 0.0f;
	identityMatrix[5] = 1.0f;
	identityMatrix[6] = 0.0f;
	identityMatrix[7] = 0.0f;
	identityMatrix[8] = 0.0f;
	identityMatrix[9] = 0.0f;
	identityMatrix[10] = 1.0f;
	identityMatrix[11] = 0.0f;
	identityMatrix[12] = 0.0f;
	identityMatrix[13] = 0.0f;
	identityMatrix[14] = 0.0f;
	identityMatrix[15] = 1.0f;

	// Translation matrix
	translationMatrix[0] = 1.0f;
	translationMatrix[1] = 0.0f;
	translationMatrix[2] = 0.0f;
	translationMatrix[3] = 0.0f;
	translationMatrix[4] = 0.0f;
	translationMatrix[5] = 1.0f;
	translationMatrix[6] = 0.0f;
	translationMatrix[7] = 0.0f;
	translationMatrix[8] = 0.0f;
	translationMatrix[9] = 0.0f;
	translationMatrix[10] = 1.0f;
	translationMatrix[11] = 0.0f;
	translationMatrix[12] = 0.0f;
	translationMatrix[13] = 0.0f;
	translationMatrix[14] = -6.0f;
	translationMatrix[15] = 1.0f;

	// Scale matrix
	scaleMatrix[0] = 0.75f;
	scaleMatrix[1] = 0.0f;
	scaleMatrix[2] = 0.0f;
	scaleMatrix[3] = 0.0f;
	scaleMatrix[4] = 0.0f;
	scaleMatrix[5] = 0.75f;
	scaleMatrix[6] = 0.0f;
	scaleMatrix[7] = 0.0f;
	scaleMatrix[8] = 0.0f;
	scaleMatrix[9] = 0.0f;
	scaleMatrix[10] = 0.75f;
	scaleMatrix[11] = 0.0f;
	scaleMatrix[12] = 0.0f;
	scaleMatrix[13] = 0.0f;
	scaleMatrix[14] = 0.0f;
	scaleMatrix[15] = 1.0f;

	// Warmap resize
	resize(WIN_WIDTH, WIN_HEIGHT);

	return (0);
}

void printGLInfo(void)
{
	// Code
	
	// Print OpenGL information

	fprintf(gpFile, "--- OPENGL INFORMATION ---\n");
	fprintf(gpFile, "*****************************\n\n");

	fprintf(gpFile, "OpenGL Vendor : %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version : %s\n\n", glGetString(GL_VERSION));

	fprintf(gpFile, "*****************************\n");
}

void resize(int width, int height)
{
	// Code

	// If height by accsident is less than 0 then height should be 1
	if (height <= 0)
	{
		height = 1;
	}

	// Set the viewport
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	// Set matrix projection mode
	glMatrixMode(GL_PROJECTION);

	// Set to identity matrix
	glLoadIdentity();

	// Do perspective projection
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

void display(void)
{
	// Variable declaration's
	float angle = 0.0f;


	// Code

	// Clear OpenGl buufers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set matrix to modelview mode
	glMatrixMode(GL_MODELVIEW);
	 
	// Set to identity matrix
	//glLoadIdentity();
	glLoadMatrixf(identityMatrix);

	// Translate triangle backwords by z
	//glTranslatef(0.0f, 0.0f, -6.0f);
	glMultMatrixf(translationMatrix);

	// Scale the triangle
	 //glScalef(0.75f, 0.75f, 0.75f);
	glMultMatrixf(scaleMatrix);

	// Rotate triangle
	/*glRotatef(angleCube, 1.0f, 0.0f, 0.0f);
	glRotatef(angleCube, 0.0f, 1.0f, 0.0f);
	glRotatef(angleCube, 0.0f, 0.0f, 1.0f);*/

	angle = angleCube * (M_PI / 180.0f);
	
	// X-Rotation matrix
	rotationMatrixX[0] = 1.0f;
	rotationMatrixX[1] = 0.0f;
	rotationMatrixX[2] = 0.0f;
	rotationMatrixX[3] = 0.0f;
	rotationMatrixX[4] = 0.0f;
	rotationMatrixX[5] = cos(angle);
	rotationMatrixX[6] = sin(angle);
	rotationMatrixX[7] = 0.0f;
	rotationMatrixX[8] = 0.0f;
	rotationMatrixX[9] = -sin(angle);
	rotationMatrixX[10] = cos(angle);
	rotationMatrixX[11] = 0.0f;
	rotationMatrixX[12] = 0.0f;
	rotationMatrixX[13] = 0.0f;
	rotationMatrixX[14] = 0.0f;
	rotationMatrixX[15] = 1.0f;

	glMultMatrixf(rotationMatrixX);

	// Y-Rotation matrix
	rotationMatrixY[0] = cos(angle);
	rotationMatrixY[1] = 0.0f;
	rotationMatrixY[2] = -sin(angle);
	rotationMatrixY[3] = 0.0f;
	rotationMatrixY[4] = 0.0f;
	rotationMatrixY[5] = 1.0f;
	rotationMatrixY[6] = 0.0f;
	rotationMatrixY[7] = 0.0f;
	rotationMatrixY[8] = sin(angle);
	rotationMatrixY[9] = 0.0f;
	rotationMatrixY[10] = cos(angle);
	rotationMatrixY[11] = 0.0f;
	rotationMatrixY[12] = 0.0f;
	rotationMatrixY[13] = 0.0f;
	rotationMatrixY[14] = 0.0f;
	rotationMatrixY[15] = 1.0f;
	
	glMultMatrixf(rotationMatrixY);

	// Z-Rotation matrix
	rotationMatrixZ[0] = cos(angle);
	rotationMatrixZ[1] = sin(angle);
	rotationMatrixZ[2] = 0.0f;
	rotationMatrixZ[3] = 0.0f;
	rotationMatrixZ[4] = -sin(angle);
	rotationMatrixZ[5] = cos(angle);
	rotationMatrixZ[6] = 0.0f;
	rotationMatrixZ[7] = 0.0f;
	rotationMatrixZ[8] = 0.0f;
	rotationMatrixZ[9] = 0.0f;
	rotationMatrixZ[10] = 1.0f;
	rotationMatrixZ[11] = 0.0f;
	rotationMatrixZ[12] = 0.0f;
	rotationMatrixZ[13] = 0.0f;
	rotationMatrixZ[14] = 0.0f;
	rotationMatrixZ[15] = 1.0f;

	glMultMatrixf(rotationMatrixZ);

	// Triangle drawing code
	glBegin(GL_QUADS);

	// Font face

	// Right top
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	// Left top
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);

	// Left bottom
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	// Right bottom
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);


	// Right face

	// Right top
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);

	// Left top
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	// Left bottom
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	// Right bottom
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);


	// Back face

	// Right top
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);

	// Left top
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);

	// Left bottom
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	// Right bottom
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);


	// Left face

	// Right top
	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);

	// Left top
	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);

	// Left bottom
	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);

	// Right bottom
	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	// Top face

	// Right top
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);

	// Left top
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);

	// Right top
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);

	// Right bottom
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);


	// Bottom face

	// Right top
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	// Left top
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);

	// Left bottom
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	// Right bottom
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	glEnd();

	// Swap the buffers
	SwapBuffers(ghdc);
}

void update(void)
{
	// Code

	angleCube = angleCube + 0.01f;
	if (angleCube >= 360.0f)
	{
		angleCube = angleCube - 360.0f;
	}
}

void uninitialize(void)
{
	// Local Function declaration's
	void toggleFullScreen(void);

	// Code

	// If user is exiting in full screen then restore the full screen back to normal
	if (gbFullScreen == TRUE)
	{
		toggleFullScreen();

		gbFullScreen = FALSE;
	}

	// Make hdc as current context by relising rendering context as current context
	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	// Delete the rendering context
	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	// Release the DC
	if (ghdc)
	{
		ReleaseDC(ghWnd, ghdc);
		ghdc = NULL;
	}

	// Destroy window
	if (ghWnd)
	{
		DestroyWindow(ghWnd);
		ghWnd = NULL;
	}

	// Close the file
	if (gpFile)
	{
		fprintf(gpFile, "\n\n");
		fprintf(gpFile, "Program terminated successfully...!\n");
		fclose(gpFile);
		gpFile = NULL;
	}

}

