// Win32 / Standard header file's
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

#include <mmsystem.h> // For PlaySound

#include <time.h> // For time() and difftime()

// OpgnGl releated header file's
#include <gl/GL.h>
#include <gl/GLU.h>

// Stb library
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"


// Custom header file's
#include "OGL.h"

// OpglGl releated librarie's
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "GLU32.lib")

#pragma comment(lib, "winmm.lib") // Link with winmm.lib

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
GLfloat anglePyramid = 0.0f;


float eyex = 0.0f;
float eyey = 0.0f;
float eyez = 4.0f;

float centerx = 0.0f;
float centery = 0.0f;
float centerz = 0.0f;

float x1 = 1.4f, y1 = 0.8f;
float x2 = -1.4f, y2 = 0.8f;
float x3 = -1.4f, y3 = -0.8f;
float x4 = 1.4f, y4 = -0.8f;

GLuint textureArray[20];
int currentTexture = 0;
int currentTexture1 = 0;

time_t start_time, end_time;
double pass_time;

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
		fprintf(gpFile, "Initialize() completed successfully...!.");
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

		//Sleep(1000);
		PlaySound(TEXT("ProjectSong.wav"), NULL, SND_ASYNC | SND_FILENAME);

		time(&start_time);

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
	BOOL loadGLTexture(GLuint*, const char*);

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

	// Load AstroMediComp textures
	if (loadGLTexture(textureArray + currentTexture, "AstroMediComp.jpg") == FALSE)
	{
		fprintf(gpFile, "ERROR : loadGLTexture() failed to create Himalaya texture...!\n");
		return(-6);
	}
	currentTexture = currentTexture + 1;

	// Load India textures
	if (loadGLTexture(textureArray + currentTexture, "India.jpg") == FALSE)
	{
		fprintf(gpFile, "ERROR : loadGLTexture() failed to create HindiMahasagar texture...!\n");
		return(-7);
	}
	currentTexture = currentTexture + 1;

	// Load Himalaya textures
	if (loadGLTexture(textureArray + currentTexture, "Himalaya.jpg") == FALSE)
	{
		fprintf(gpFile, "ERROR : loadGLTexture() failed to create Gangaghat texture...!\n");
		return(-8);
	}
	currentTexture = currentTexture + 1;

	// Load HindiMahasagar textures
	if (loadGLTexture(textureArray + currentTexture, "HindiMahasagar.jpg") == FALSE)
	{
		fprintf(gpFile, "ERROR : loadGLTexture() failed to create Gangaghat texture...!\n");
		return(-8);
	}
	currentTexture = currentTexture + 1;

	// Load gangaghat textures
	if (loadGLTexture(textureArray + currentTexture, "Gangaghat.jpg") == FALSE)
	{
		fprintf(gpFile, "ERROR : loadGLTexture() failed to create Gangaghat texture...!\n");
		return(-8);
	}
	currentTexture = currentTexture + 1;

	// Load AllPart textures
	if (loadGLTexture(textureArray + currentTexture, "AllPart.jpg") == FALSE)
	{
		fprintf(gpFile, "ERROR : loadGLTexture() failed to create Gangaghat texture...!\n");
		return(-8);
	}
	currentTexture = currentTexture + 1;

	// Load Singer textures
	if (loadGLTexture(textureArray + currentTexture, "Singer.jpg") == FALSE)
	{
		fprintf(gpFile, "ERROR : loadGLTexture() failed to create Gangaghat texture...!\n");
		return(-8);
	}
	currentTexture = currentTexture + 1;

	// Load Technical Details textures
	if (loadGLTexture(textureArray + currentTexture, "Technical.jpg") == FALSE)
	{
		fprintf(gpFile, "ERROR : loadGLTexture() failed to create Gangaghat texture...!\n");
		return(-8);
	}
	currentTexture = currentTexture + 1;

	// Load Name textures
	if (loadGLTexture(textureArray + currentTexture, "myName.jpg") == FALSE)
	{
		fprintf(gpFile, "ERROR : loadGLTexture() failed to create Gangaghat texture...!\n");
		return(-8);
	}
	currentTexture = currentTexture + 1;

	// Load Guidance textures
	if (loadGLTexture(textureArray + currentTexture, "Guidance.jpg") == FALSE)
	{
		fprintf(gpFile, "ERROR : loadGLTexture() failed to create Gangaghat texture...!\n");
		return(-8);
	}
	currentTexture = currentTexture + 1;

	// Load Sir textures
	if (loadGLTexture(textureArray + currentTexture, "Sir.jpg") == FALSE)
	{
		fprintf(gpFile, "ERROR : loadGLTexture() failed to create Gangaghat texture...!\n");
		return(-8);
	}
	currentTexture = currentTexture + 1;

	// Enable textureing
	glEnable(GL_TEXTURE_2D);

	// Warmap resize
	resize(WIN_WIDTH, WIN_HEIGHT);


	toggleFullScreen();
	gbFullScreen = TRUE;

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

BOOL loadGLTexture(GLuint* texture, const char* fileName)
{
	// Variable declaraton's
	BOOL bResult = TRUE;

	// Code

	int w, h, n;
	unsigned char* data = stbi_load(fileName, &w, &h, &n, 4);
	if (data == NULL)
	{
		bResult = FALSE;
	}

	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);

	return(bResult);
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
	// Code

	// Clear OpenGl buufers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set matrix to modelview mode
	glMatrixMode(GL_MODELVIEW);

	// Set to identity matrix
	glLoadIdentity();

	// Translate triangle backword by 3
	glTranslatef(0.0f, 0.0f, -1.0f);

	glPushMatrix();

	gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, 0.0f, 1.0f, 0.0f);


	glBindTexture(GL_TEXTURE_2D, textureArray[currentTexture1]);
	glBegin(GL_QUADS);

	// Right top
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(x1, y1, 0.0f);

	// Left top
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(x2, y2, 0.0f);

	// Left bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(x3, y3, 0.0f);

	// Right bottom
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(x4, y4, 0.0f);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();


	// Swap the buffers
	SwapBuffers(ghdc);
}

void update(void)
{
	// Code

	if (currentTexture1 == 0)
	{
		eyey = eyey + 0.00000001;
		if (eyey >= 5.0f)
		{
			eyey = 0.5;
		}

		eyez = eyez - 0.0003;

		if (eyez <= 0.55f)
		{
			eyez = 0.55f;

			time(&end_time);
			pass_time = difftime(end_time, start_time);

			if (pass_time > 10)
			{
				eyez = 4.5f;

				time(&start_time);
				currentTexture1 = 1;
			}
		}
	}
	else if (currentTexture1 == 1)
	{
		eyez = eyez - 0.0001;

		if (eyez <= 0.6f)
		{
			eyez = 0.6f;

			time(&end_time);
			pass_time = difftime(end_time, start_time);

			if (pass_time > 38.0f)
			{
				eyez = 0.5f;

				currentTexture1 = 2;
			}
		}
	}
	else if (currentTexture1 == 2)
	{
		eyez = eyez - 0.00001;
		eyex = eyex - 0.00001;
		centerx = centerx - 0.00002f;

		if (eyex <= -0.05f)
		{
			eyex = 0.0f;
			centerx = 0.0f;

			eyez = 0.6f;
			currentTexture1 = 3;
		}
	}
	else if (currentTexture1 == 3)
	{
		centerx = centerx + 0.0000001f;
		eyex = eyex - 0.00002;
		if (eyex <= -0.09f)
		{
			time(&start_time);
			eyex = -0.09f;
			currentTexture1 = 4;
		}
	}
	else if (currentTexture1 == 4)
	{
		centerx = centerx + 0.0000001f;
		eyex = eyex - 0.00001;
		if (eyex <= -0.11f)
		{
			eyex = -0.11f;

			time(&end_time);
			pass_time = difftime(end_time, start_time);

			if (pass_time > 8.0f)
			{
				eyex = 0.0f;

				time(&start_time);
				currentTexture1 = 5;
			}
		}
	}
	else if (currentTexture1 == 5)
	{
		time(&end_time);
		pass_time = difftime(end_time, start_time);

		if (pass_time > 7.5f)
		{
			eyez = 1.0f;
			centerx = 0.0f;
			centery = 0.0f;
			centerz = 0.0f;
			time(&start_time);
			currentTexture1 = 6;
		}
	}
	else if (currentTexture1 == 6)
	{
		time(&end_time);
		pass_time = difftime(end_time, start_time);

		if (pass_time > 2.5f)
		{
			time(&start_time);
			currentTexture1 = 7;
		}
	}
	else if (currentTexture1 == 7)
	{
		time(&end_time);
		pass_time = difftime(end_time, start_time);

		if (pass_time > 2.5f)
		{
			time(&start_time);
			currentTexture1 = 8;
		}
	}
	else if (currentTexture1 == 8)
	{
		time(&end_time);
		pass_time = difftime(end_time, start_time);

		if (pass_time > 2.5f)
		{
			time(&start_time);
			currentTexture1 = 9;
		}
	}
	else if (currentTexture1 == 9)
	{
		time(&end_time);
		pass_time = difftime(end_time, start_time);

		if (pass_time > 2.5f)
		{
			time(&start_time);
			currentTexture1 = 10;
		}
	}
	else if (currentTexture1 == 10)
	{
		time(&end_time);
		pass_time = difftime(end_time, start_time);

		if (pass_time > 2.5f)
		{
			time(&start_time);

			uninitialize();
			exit(0);
		}
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

	// Free textures
	for (int i = currentTexture; i > -1; --i)
	{
		glDeleteTextures(1, textureArray + currentTexture);
		textureArray[currentTexture] = 0;
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
		fprintf(gpFile, "Program terminated successfully...!\n");
		fclose(gpFile);
		gpFile = NULL;
	}

}

