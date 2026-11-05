// Win32 / Standard header file's
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

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

// Sphere releated variable's
GLUquadric* quadric = NULL;

// Variable's for 24 Sphere Application
BOOL bLight = FALSE;
GLfloat LightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat LightDiffuce[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightSpeqular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat LightModelAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat LightModelLocalViewer[] = { 0.0f };

GLfloat AngleForXRotation = 0.0f;
GLfloat AngleForYRotation = 0.0f;
GLfloat AngleForZRotation = 0.0f;

GLint Keypressed = -1;

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

		case 'L':
		case 'l':
			if (bLight == FALSE)
			{
				bLight = TRUE;

				glEnable(GL_LIGHTING);
			}
			else
			{
				bLight = FALSE;

				glDisable(GL_LIGHTING);
			}
			break;

		case 'X':
		case 'x':
			Keypressed = 1;
			AngleForXRotation = 0.0f;   // Reseting angle for 'X' Rotation
			break;

		case 'Y':
		case 'y':
			Keypressed = 2;
			AngleForYRotation = 0.0f;    // Reseting angle for 'Y' Rotation
			break;

		case 'Z':
		case 'z':
			Keypressed = 3;
			AngleForZRotation = 0.0f;     // Reseting angle for 'Z' Rotation
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
	glClearColor(0.50f, 0.50f, 0.50f, 1.0f);

	// Initialize quadric
	quadric = gluNewQuadric();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuce);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpeqular);
	
	glEnable(GL_LIGHT0);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, LightModelLocalViewer);

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

	// Do orthographic projection
	if (width <= height)
	{
		glOrtho(0.0f,
			15.5f,
			(0.0f * ((GLfloat)height / (GLfloat)width)),
			(15.5f * ((GLfloat)height / (GLfloat)width)),
			-10.0f,
			10.0f
		);
	}
	else
	{
		glOrtho((0.0 * ((GLfloat)width / (GLfloat)height)),
			(15.5f * ((GLfloat)width / (GLfloat)height)),
			0.0f,
			15.5f,
			-10.0f,
			10.0f
		);
	}
}

void display(void)
{
	// Local function declaration's
	void draw24Spheres(void);

	// Code

	// Clear OpenGl buufers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set matrix to modelview mode
	glMatrixMode(GL_MODELVIEW);
	 
	// Set to identity matrix
	glLoadIdentity();

	if (Keypressed == 1)
	{
		glRotatef(AngleForXRotation, 1.0f, 0.0f, 0.0f);
		LightPosition[2] = AngleForXRotation;
	}
	else if (Keypressed == 2)
	{
		glRotatef(AngleForYRotation, 0.0f, 1.0f, 0.0f);
		LightPosition[0] = AngleForYRotation;
	}
	else if(Keypressed == 3)
	{
		glRotatef(AngleForZRotation, 0.0f, 0.0f, 1.0f);
		LightPosition[1] = AngleForZRotation;
	}

	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	// Draw 24-Sphere's
	draw24Spheres();

	// Swap the buffers
	SwapBuffers(ghdc);
}

void draw24Spheres(void)
{
	 // Variable declaration's
	GLfloat MaterialAmbient[4];
	GLfloat MaterialDiffuce[4];
	GLfloat MaterialSpeqular[4];
	GLfloat MaterialShineness;

	// Code

	// 1st Spheres Of 1st Column  **Emerald**
	MaterialAmbient[0] = 0.0215f;
	MaterialAmbient[1] = 0.1745f;
	MaterialAmbient[2] = 0.0215f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.07568f;
	MaterialDiffuce[1] = 0.61424f;
	MaterialDiffuce[2] = 0.07568f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.633f;
	MaterialSpeqular[1] = 0.727811f;
	MaterialSpeqular[2] = 0.633f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.6f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(3.5f, 14.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	// 2nd Spheres Of 1st Column  **Jade**
	MaterialAmbient[0] = 0.135f;
	MaterialAmbient[1] = 0.2225f;
	MaterialAmbient[2] = 0.1575f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.54f;
	MaterialDiffuce[1] = 0.89f;
	MaterialDiffuce[2] = 0.63f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.316228f;
	MaterialSpeqular[1] = 0.316228f;
	MaterialSpeqular[2] = 0.316228f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.1f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(3.5f, 11.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	// 3rd Spheres Of 1st Column  **Obsidian**
	MaterialAmbient[0] = 0.05375f;
	MaterialAmbient[1] = 0.05f;
	MaterialAmbient[2] = 0.06625f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.18275f;
	MaterialDiffuce[1] = 0.17f;
	MaterialDiffuce[2] = 0.22525f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.332741f;
	MaterialSpeqular[1] = 0.328634f;
	MaterialSpeqular[2] = 0.346435f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.3f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(3.5f, 9.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	// 4th Spheres Of 1st Column  **Pearl**
	MaterialAmbient[0] = 0.25f;
	MaterialAmbient[1] = 0.20725f;
	MaterialAmbient[2] = 0.20725f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 1.0f;
	MaterialDiffuce[1] = 0.829f;
	MaterialDiffuce[2] = 0.829f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.296648f;
	MaterialSpeqular[1] = 0.296648f;
	MaterialSpeqular[2] = 0.296648f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.088f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(3.5f, 6.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	// 5th Spheres Of 1st Column  **Ruby**
	MaterialAmbient[0] = 0.1745f;
	MaterialAmbient[1] = 0.01175f;
	MaterialAmbient[2] = 0.01175f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.61424f;
	MaterialDiffuce[1] = 0.04136f;
	MaterialDiffuce[2] = 0.04136f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.727811f;
	MaterialSpeqular[1] = 0.626959f;
	MaterialSpeqular[2] = 0.626959f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.6f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(3.5f, 4.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	// 6th Spheres Of 1st Column  **Turquoise**
	MaterialAmbient[0] = 0.1f;
	MaterialAmbient[1] = 0.18725f;
	MaterialAmbient[2] = 0.1745f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.396f;
	MaterialDiffuce[1] = 0.74151f;
	MaterialDiffuce[2] = 0.69102f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.297254f;
	MaterialSpeqular[1] = 0.30829f;
	MaterialSpeqular[2] = 0.306678f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.1f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(3.5f, 1.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	//*********************************************************
	//*********************************************************
	//*********************************************************
	//*********************************************************

	// 1st Spheres Of 2nd Column  **Brass**
	MaterialAmbient[0] = 0.329412f;
	MaterialAmbient[1] = 0.223529f;
	MaterialAmbient[2] = 0.027451;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.780392f;
	MaterialDiffuce[1] = 0.568627f;
	MaterialDiffuce[2] = 0.113725f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.992157f;
	MaterialSpeqular[1] = 0.941176f;
	MaterialSpeqular[2] = 0.807843f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.21794872f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(9.5f, 14.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	// 2nd Spheres Of 2nd Column  **Bronze**
	MaterialAmbient[0] = 0.2125f;
	MaterialAmbient[1] = 0.1275f;
	MaterialAmbient[2] = 0.054f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.714f;
	MaterialDiffuce[1] = 0.4284f;
	MaterialDiffuce[2] = 0.18144f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.393548f;
	MaterialSpeqular[1] = 0.271906f;
	MaterialSpeqular[2] = 0.166721f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.2f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(9.5f, 11.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	// 3rd Spheres Of 2nd Column  **Chrome**
	MaterialAmbient[0] = 0.25f;
	MaterialAmbient[1] = 0.25f;
	MaterialAmbient[2] = 0.25f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.4f;
	MaterialDiffuce[1] = 0.4f;
	MaterialDiffuce[2] = 0.4f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.774597f;
	MaterialSpeqular[1] = 0.774597f;
	MaterialSpeqular[2] = 0.774597f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.6f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(9.5f, 9.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	// 4th Spheres Of 2nd Column  **Copper**
	MaterialAmbient[0] = 0.19125f;
	MaterialAmbient[1] = 0.0735f;
	MaterialAmbient[2] = 0.0225f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.7038f;
	MaterialDiffuce[1] = 0.27048f;
	MaterialDiffuce[2] = 0.0828f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.256777f;
	MaterialSpeqular[1] = 0.137622f;
	MaterialSpeqular[2] = 0.086014f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.1f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(9.5f, 6.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	// 5th Spheres Of 2nd Column  **Gold**
	MaterialAmbient[0] = 0.24725f;
	MaterialAmbient[1] = 0.1995f;
	MaterialAmbient[2] = 0.0745f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.075164f;
	MaterialDiffuce[1] = 0.60648f;
	MaterialDiffuce[2] = 0.22648f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.628281f;
	MaterialSpeqular[1] = 0.555802f;
	MaterialSpeqular[2] = 0.366065f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.4f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(9.5f, 4.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	// 6th Spheres Of 2nd Column  **Silver**
	MaterialAmbient[0] = 0.19225f;
	MaterialAmbient[1] = 0.19225f;
	MaterialAmbient[2] = 0.19225f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.50754f;
	MaterialDiffuce[1] = 0.50754f;
	MaterialDiffuce[2] = 0.50754f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.508273f;
	MaterialSpeqular[1] = 0.508273f;
	MaterialSpeqular[2] = 0.508273f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.4f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(9.5f, 1.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************
	 
	//*********************************************************
	//*********************************************************
	//*********************************************************
	//*********************************************************

	// 1st Spheres Of 3rd Column  **Black**
	MaterialAmbient[0] = 0.0f;
	MaterialAmbient[1] = 0.0f;
	MaterialAmbient[2] = 0.0f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.01f;
	MaterialDiffuce[1] = 0.01f;
	MaterialDiffuce[2] = 0.01f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.50f;
	MaterialSpeqular[1] = 0.50f;
	MaterialSpeqular[2] = 0.50f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.25f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(15.5f, 14.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	// 2nd Spheres Of 3rd Column  **Cyne**
	MaterialAmbient[0] = 0.0f;
	MaterialAmbient[1] = 0.1f;
	MaterialAmbient[2] = 0.06f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.0f;
	MaterialDiffuce[1] = 0.50980392f;
	MaterialDiffuce[2] = 0.50980392f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.50196078f;
	MaterialSpeqular[1] = 0.50196078f;
	MaterialSpeqular[2] = 0.50196078f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.25f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(15.5f, 11.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	// 3rd Spheres Of 3rd Column  **Green**
	MaterialAmbient[0] = 0.0f;
	MaterialAmbient[1] = 0.0f;
	MaterialAmbient[2] = 0.0f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.1f;
	MaterialDiffuce[1] = 0.35f;
	MaterialDiffuce[2] = 0.1f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.45f;
	MaterialSpeqular[1] = 0.55f;
	MaterialSpeqular[2] = 0.45f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.25f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(15.5f, 9.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	// 4th Spheres Of 3rd Column  **Red**
	MaterialAmbient[0] = 0.0f;
	MaterialAmbient[1] = 0.0f;
	MaterialAmbient[2] = 0.0f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.5f;
	MaterialDiffuce[1] = 0.0f;
	MaterialDiffuce[2] = 0.0f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.7f;
	MaterialSpeqular[1] = 0.6f;
	MaterialSpeqular[2] = 0.6f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.25f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(15.5f, 6.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	// 5th Spheres Of 3rd Column  **White**
	MaterialAmbient[0] = 0.0f;
	MaterialAmbient[1] = 0.0f;
	MaterialAmbient[2] = 0.0f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.55f;
	MaterialDiffuce[1] = 0.55f;
	MaterialDiffuce[2] = 0.55f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.70f;
	MaterialSpeqular[1] = 0.70f;
	MaterialSpeqular[2] = 0.70f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.25f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(15.5f, 4.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	// 6th Spheres Of 3rd Column  **Yellow**
	MaterialAmbient[0] = 0.0f;
	MaterialAmbient[1] = 0.0f;
	MaterialAmbient[2] = 0.0f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.5f;
	MaterialDiffuce[1] = 0.5f;
	MaterialDiffuce[2] = 0.0f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.60f;
	MaterialSpeqular[1] = 0.60f;
	MaterialSpeqular[2] = 0.50f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.25f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(15.5f, 1.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	//*********************************************************
	//*********************************************************
	//*********************************************************
	//*********************************************************

	// 1st Spheres Of 4th Column  **Black**
	MaterialAmbient[0] = 0.002f;
	MaterialAmbient[1] = 0.002f;
	MaterialAmbient[2] = 0.002f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.01f;
	MaterialDiffuce[1] = 0.01f;
	MaterialDiffuce[2] = 0.01f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.4f;
	MaterialSpeqular[1] = 0.4f;
	MaterialSpeqular[2] = 0.4f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.078125f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(21.5f, 14.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	// 2nd Spheres Of 4th Column  **Cyne**
	MaterialAmbient[0] = 0.0f;
	MaterialAmbient[1] = 0.05f;
	MaterialAmbient[2] = 0.05f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.4f;
	MaterialDiffuce[1] = 0.5f;
	MaterialDiffuce[2] = 0.5f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.04f;
	MaterialSpeqular[1] = 0.7f;
	MaterialSpeqular[2] = 0.7f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.078125f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(21.5f, 11.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	// 3rd Spheres Of 4th Column  **Green**
	MaterialAmbient[0] = 0.0f;
	MaterialAmbient[1] = 0.05f;
	MaterialAmbient[2] = 0.0f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.4f;
	MaterialDiffuce[1] = 0.5f;
	MaterialDiffuce[2] = 0.4f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.04f;
	MaterialSpeqular[1] = 0.7f;
	MaterialSpeqular[2] = 0.04f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.078125f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(21.5f, 9.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	// 4th Spheres Of 4th Column  **Red**
	MaterialAmbient[0] = 0.05f;
	MaterialAmbient[1] = 0.0f;
	MaterialAmbient[2] = 0.0f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.5f;
	MaterialDiffuce[1] = 0.4f;
	MaterialDiffuce[2] = 0.4f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.7f;
	MaterialSpeqular[1] = 0.04f;
	MaterialSpeqular[2] = 0.04f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.078125f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(21.5f, 6.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	// 5th Spheres Of 4th Column  **White**
	MaterialAmbient[0] = 0.05f;
	MaterialAmbient[1] = 0.05f;
	MaterialAmbient[2] = 0.05f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.5f;
	MaterialDiffuce[1] = 0.5f;
	MaterialDiffuce[2] = 0.5f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.7f;
	MaterialSpeqular[1] = 0.7f;
	MaterialSpeqular[2] = 0.7f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.078125f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(21.5f, 4.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	//*********************************************************

	// 6th Spheres Of 4th Column  **Yellow**
	MaterialAmbient[0] = 0.05f;
	MaterialAmbient[1] = 0.05f;
	MaterialAmbient[2] = 0.0f;
	MaterialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuce[0] = 0.5f;
	MaterialDiffuce[1] = 0.5f;
	MaterialDiffuce[2] = 0.4f;
	MaterialDiffuce[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuce);

	MaterialSpeqular[0] = 0.7f;
	MaterialSpeqular[1] = 0.7f;
	MaterialSpeqular[2] = 0.04f;
	MaterialSpeqular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpeqular);

	MaterialShineness = 0.078125f * 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(21.5f, 1.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);

	//*********************************************************
	//*********************************************************
	//*********************************************************
	//*********************************************************
}

void update(void)
{
	// Code

	AngleForXRotation = AngleForXRotation + 0.3f;
	if (AngleForXRotation >= 360.0f)
	{
		AngleForXRotation = AngleForXRotation - 360.0f;
	}

	AngleForYRotation = AngleForYRotation + 0.3f;
	if (AngleForYRotation >= 360.0f)
	{
		AngleForYRotation = AngleForYRotation - 360.0f;
	}

	AngleForZRotation = AngleForZRotation + 0.3f;
	if (AngleForZRotation >= 360.0f)
	{
		AngleForZRotation = AngleForZRotation - 360.0f;
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

	// Delete the quadric
	if (quadric)
	{
		gluDeleteQuadric(quadric);
		quadric = NULL;
	}

	// Close the file
	if (gpFile)
	{
		fprintf(gpFile, "Program terminated successfully...!\n");
		fclose(gpFile);
		gpFile = NULL;
	}

}

