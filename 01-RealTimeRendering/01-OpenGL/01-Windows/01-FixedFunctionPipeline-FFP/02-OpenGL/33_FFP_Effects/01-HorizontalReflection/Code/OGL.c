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

#define MAX_LIMIT_CUBE_TRANSLATE_X	8.0f
#define MIN_LIMIT_CUBE_TRANSLATE_X	-8.0f

#define MAX_LIMIT_CUBE_TRANSLATE_Y	4.0f
#define MIN_LIMIT_CUBE_TRANSLATE_Y	1.45f

#define CLOSES_LIMIT_CUBE_TRANSLATE_Z 5.0f
#define FARTHEST_LIMIT_CUBE_TRANSLATE_Z	-20.0f

#define INCREMENT	0.05f;
#define DECREMENT	0.05f;

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

// Variable releated with light's
GLfloat lightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat lightDiffuce[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpeqular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 0.0f, 5.0f, 10.0f, 1.0f };

GLfloat materialAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat materialDiffuce[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialSpeqular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialShininess = 128.0f;

// Special effect releated variable's
GLuint texture_Marble;

GLfloat translateCubeX = 0.0f;
GLfloat translateCubeY = 1.45f;
GLfloat translateCubeZ = 0.0f;

GLfloat CubeScale = 0.25f;

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

		case VK_RIGHT:      // To translate cube on positive X-axis
			if (translateCubeX < MAX_LIMIT_CUBE_TRANSLATE_X)
			{
				translateCubeX = translateCubeX + INCREMENT;
			}
			break;

		case VK_LEFT:		// To translate cube on negative X-axis
			if (translateCubeX > MIN_LIMIT_CUBE_TRANSLATE_X)
			{
				translateCubeX = translateCubeX - DECREMENT;
			}
			break;

		case VK_UP:         // To translate cube on positive Y-axis
			if (translateCubeY < MAX_LIMIT_CUBE_TRANSLATE_Y)
			{
				translateCubeY = translateCubeY + INCREMENT;
			}
			break;

		case VK_DOWN:        // To translate cube Downwords
			if (translateCubeY > MIN_LIMIT_CUBE_TRANSLATE_Y)
			{
				translateCubeY = translateCubeY - DECREMENT;
			}
			break;

		case VK_ADD:		// To translate cube on positive Z-axis that is towords the viewer
			if (translateCubeZ < CLOSES_LIMIT_CUBE_TRANSLATE_Z)
			{
				translateCubeZ = translateCubeZ + INCREMENT;
			}
			break;

		case VK_SUBTRACT:    // To translate cube on positive Z - axis that is away the viewer
			if (translateCubeZ > FARTHEST_LIMIT_CUBE_TRANSLATE_Z)
			{
				translateCubeZ = translateCubeZ - DECREMENT;
			}
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
	BOOL loadGLTexture(GLuint * texture, TCHAR imageResourceID[]);

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
	pfd.cStencilBits = 32;

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

	// Load textures
	if (loadGLTexture(&texture_Marble, MAKEINTRESOURCE(IDBITMAP_MARBLE)) == FALSE)
	{
		fprintf(gpFile, "ERROR : loadGLTexture() failed to create marble_bmp...!\n");
		return(-6);
	}

	// Enable textureing
	glEnable(GL_TEXTURE_2D);

	// Light configuration
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuce);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpeqular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT0);

	glEnable(GL_NORMALIZE);
	glEnable(GL_AUTO_NORMAL);

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuce);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpeqular);
	glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);

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

BOOL loadGLTexture(GLuint* texture, TCHAR imageResourceID[])
{
	// Variable declaraton's
	HBITMAP hBitmap = NULL;
	BITMAP bmp;
	BOOL bResult = FALSE;

	// Code

	// Load the bitmap as image
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (hBitmap)
	{
		bResult = TRUE;

		// Get bitmap structure from the loaded bitmpa image
		GetObject(hBitmap, sizeof(BITMAP), &bmp);

		// Generate OpenGL texture object
		glGenTextures(1, texture);

		// Bind to the newly created empty structure object
		glBindTexture(GL_TEXTURE_2D, *texture);

		// Unpack the image in to memory for faster loading
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);

		glBindTexture(GL_TEXTURE_2D, 0);

		DeleteObject(hBitmap);
		hBitmap = NULL;
	}

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
	// Local function declaration's
	void drawLitCube(void);
	void drawFloor(void);

	// Code

	// Clear OpenGl buufers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Set matrix to modelview mode
	glMatrixMode(GL_MODELVIEW);

	// Set to identity matrix
	glLoadIdentity();

	// Setup Camera
	gluLookAt(0.0f, 5.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Render the actual cube 
	glPushMatrix();

	// Translate triangle backwords by z
	glTranslatef(translateCubeX, translateCubeY, translateCubeZ);

	glScalef(CubeScale, CubeScale, CubeScale);

	// Rotate triangle
	glRotatef(angleCube, 1.0f, 0.0f, 0.0f);
	glRotatef(angleCube, 0.0f, 1.0f, 0.0f);
	glRotatef(angleCube, 0.0f, 0.0f, 1.0f);

	drawLitCube();

	glPopMatrix();

	// Mazi farshi hich mazya rangolicha chap aahe
	// Define your floor as stencil

	// We defineing our floor as stencil so don't really need depth testing because for some time we do not rendering anyting
	glDisable(GL_DEPTH_TEST);

	// Mask out all the four color compont as we do not redering for now
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	// Now enable stencil test, rangolicha chap tayar karne
	glEnable(GL_STENCIL_TEST);

	// Always pass the stencil test
	// Maza rangolicha chap kasa aasava
	glStencilFunc(GL_ALWAYS, 1, 1);

	// What should i do when the stencil test passes
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// This drawFloor() call will not render the floor because Depth testing is off and all colors are masked this call only define the floor as stencil limit
	drawFloor();

	// Render the reflected cube

	// Now as we start rendring again reenable the depth test
	glEnable(GL_DEPTH_TEST);

	// Now as we start rendring again let the colors come
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	// Draw only where there is store 1 in stencil buffer
	glStencilFunc(GL_EQUAL, 1, 1);

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glPushMatrix();

	glScalef(1.0f, -1.0f, 1.0f);

	// Translate triangle backwords by z
	glTranslatef(translateCubeX, translateCubeY, translateCubeZ);

	glScalef(CubeScale, CubeScale, CubeScale);

	// Rotate triangle
	glRotatef(angleCube, 1.0f, 0.0f, 0.0f);
	glRotatef(angleCube, 0.0f, 1.0f, 0.0f);
	glRotatef(angleCube, 0.0f, 0.0f, 1.0f);

	drawLitCube();

	glPopMatrix();

	// Disable stencil test as we done with stencil test
	glDisable(GL_STENCIL_TEST);

	// Render the floor
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(1.0f, 1.0f, 1.0f, 0.7f);

	drawFloor();

	glDisable(GL_BLEND);

	// Swap the buffers
	SwapBuffers(ghdc);
}

void drawLitCube(void)
{
	// Code

	glEnable(GL_LIGHTING);

	// Triangle drawing code
	glBegin(GL_QUADS);

	// Font face

	glNormal3f(0.0f, 0.0f, 1.0f);

	// Right top
	glVertex3f(1.0f, 1.0f, 1.0f);

	// Left top
	glVertex3f(-1.0f, 1.0f, 1.0f);

	// Left bottom
	glVertex3f(-1.0f, -1.0f, 1.0f);

	// Right bottom
	glVertex3f(1.0f, -1.0f, 1.0f);


	// Right face

	glNormal3f(1.0f, 0.0f, 0.0f);

	// Right top
	glVertex3f(1.0f, 1.0f, -1.0f);

	// Left top
	glVertex3f(1.0f, 1.0f, 1.0f);

	// Left bottom
	glVertex3f(1.0f, -1.0f, 1.0f);

	// Right bottom
	glVertex3f(1.0f, -1.0f, -1.0f);


	// Back face

	glNormal3f(0.0f, 0.0f, -1.0f);

	// Right top
	glVertex3f(-1.0f, 1.0f, -1.0f);

	// Left top
	glVertex3f(1.0f, 1.0f, -1.0f);

	// Left bottom
	glVertex3f(1.0f, -1.0f, -1.0f);

	// Right bottom
	glVertex3f(-1.0f, -1.0f, -1.0f);


	// Left face

	glNormal3f(-1.0f, 0.0f, 0.0f);

	// Right top
	glVertex3f(-1.0f, 1.0f, 1.0f);

	// Left top
	glVertex3f(-1.0f, 1.0f, -1.0f);

	// Left bottom
	glVertex3f(-1.0f, -1.0f, -1.0f);

	// Right bottom
	glVertex3f(-1.0f, -1.0f, 1.0f);


	// Top face

	glNormal3f(0.0f, 1.0f, 0.0f);

	// Right top
	glVertex3f(1.0f, 1.0f, -1.0f);

	// Left top
	glVertex3f(-1.0f, 1.0f, -1.0f);

	// Right top
	glVertex3f(-1.0f, 1.0f, 1.0f);

	// Right bottom
	glVertex3f(1.0f, 1.0f, 1.0f);


	// Bottom face

	glNormal3f(0.0f, -1.0f, 0.0f);

	// Right top
	glVertex3f(1.0f, -1.0f, -1.0f);

	// Left top
	glVertex3f(-1.0f, -1.0f, -1.0f);

	// Left bottom
	glVertex3f(-1.0f, -1.0f, 1.0f);

	// Right bottom
	glVertex3f(1.0f, -1.0f, 1.0f);

	glEnd();

	glDisable(GL_LIGHTING);

}

void drawFloor(void)
{
	// Code

	// Pushing the camera matrix set in display
	glPushMatrix();

	// Translate triangle backword by 3
	glTranslatef(0.0f, -1.0f, -2.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(5.0f, 5.0f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, texture_Marble);

	// Triangle drawing code
	glBegin(GL_QUADS);

	// Right top
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);

	// Left top
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);

	// Left bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);

	// Right bottom
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();

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

	// release textrue
	if (texture_Marble)
	{
		glDeleteTextures(1, &texture_Marble);
		texture_Marble = 0;
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

