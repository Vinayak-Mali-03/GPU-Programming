// Win32 / Standard header file's
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

// OpgnGl related header file's
#include <GL/glew.h>     // This header file must be included before "gl.h"
#include <gl/GL.h>
#include "vmath.h"		// matrix related and transformation related matrix

// Custom header file's
#include "Sphere.h"

using namespace vmath;

// Custom header file's
#include "OGL.h"

// OpglGl related librarie's
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

// Custom library
#pragma comment(lib, "Sphere.lib")

// Macros
#define WIN_WIDTH	800
#define WIN_HEIGHT	600

// Global function declaration's
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Global variable declaration's

// Active window related variable's
BOOL gbActiveWindow = FALSE;

// Exit keypress related
BOOL gbEscapeKeyIsPressed = FALSE;

// Variable's related with full screen 
BOOL gbFullScreen = FALSE;
HWND ghWnd = NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev;

// Variable's related with FileI/O
char gszLogFileName[] = "Log.txt";
FILE* gpFile = NULL;

// OpenGL related global variable's
HDC ghdc = NULL;
HGLRC ghrc = NULL;

// Shader related variable's
GLuint shaderProgramObject_PV = 0;
GLuint shaderProgramObject_PF = 0;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_NORMAL,
};

GLuint vao = 0;
GLuint vbo_position = 0;
GLuint vbo_normal = 0;
GLuint vbo_element = 0;

// Uniform
GLuint modelMatrixUniform_PV = 0;
GLuint viewMatrixUniform_PV = 0;
GLuint projectionMatrixUniform_PV = 0;

GLuint modelMatrixUniform_PF = 0;
GLuint viewMatrixUniform_PF = 0;
GLuint projectionMatrixUniform_PF = 0;

// Light
GLuint laUniform_PV = 0;
GLuint ldUniform_PV = 0;
GLuint lsUniform_PV = 0;
GLuint lightPositionUniform_PV = 0;

GLuint laUniform_PF = 0;
GLuint ldUniform_PF = 0;    
GLuint lsUniform_PF = 0;
GLuint lightPositionUniform_PF = 0;

// Material
GLuint kaUniform_PV = 0;
GLuint kdUniform_PV = 0;
GLuint ksUniform_PV = 0;
GLuint materialShininessUniform_PV = 0;

GLuint kaUniform_PF = 0;
GLuint kdUniform_PF = 0;
GLuint ksUniform_PF = 0;
GLuint materialShininessUniform_PF = 0;

GLuint lKeyPressUniform_PV = 0;
GLuint lKeyPressUniform_PF = 0;

mat4 perspectiveProjectionMatrix;

BOOL bLight = FALSE;

GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuce[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpeqular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };

GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuce[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpeqular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialShininess = 50.0f;

BOOL bPerVertex = TRUE;
BOOL bPerFragment = FALSE;

// Sphere related variable's
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

unsigned int gNumVertices = 0;
unsigned int gNumElements = 0;



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

	case WM_CHAR:
		switch (wParam)
		{
		case 'Q':
		case 'q':
			gbEscapeKeyIsPressed = TRUE;
			break;

		case 'F':
		case 'f':
			bPerFragment = TRUE;
			bPerVertex = FALSE;
			break;

		case 'V':
		case 'v':
			bPerVertex = TRUE;
			bPerFragment = FALSE;
			break;

		case 'L':
		case 'l':
			if (bLight == FALSE)
			{
				bLight = TRUE;

				bPerVertex = TRUE;
				bPerFragment = FALSE;
			}
			else
			{
				bLight = FALSE;
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
	void uninitialize(void);

	// Variable declaration's
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;
	GLenum glewResult;

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

	// Initilize glew
	glewResult = glewInit();
	if (glewResult != GLEW_OK)
	{
		fprintf(gpFile, "ERROR : glewInit() Function Failed...!!!\n");
		return(-6);
	}

	// Print OpenGL info
	printGLInfo();

	//********************************** PER VERTEX *************************************

//============================================== VERTEX SHADER ====================================================
	const GLchar* vertexShaderSourceCode_PV =
		"#version 460 core\n" \
		"in vec4 aPosition;\n" \
		"in vec3 aNormal;\n" \
		"out vec3 out_Phong_ADS_Light;\n" \
		"uniform mat4 uModelMatrix;\n" \
		"uniform mat4 uViewMatrix;\n" \
		"uniform mat4 uProjectionMatrix;\n" \
		"uniform vec3 uLa;\n" \
		"uniform vec3 uLd;\n" \
		"uniform vec3 uLs;\n" \
		"uniform vec4 uLightPosition;\n" \
		"uniform vec3 uKa;\n" \
		"uniform vec3 uKd;\n" \
		"uniform vec3 uKs;\n" \
		"uniform float uMaterialShininess;\n" \
		"uniform int uLKeyIsPressed;\n" \
		"void main(void)\n" \
		"{\n" \
		"gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n" \
		"if(uLKeyIsPressed == 1)\n" \
		"{\n" \
		"vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition;\n" \
		"mat3 normalMatrix = mat3(uViewMatrix * uModelMatrix);\n" \
		"vec3 transformedNormals = normalize(normalMatrix * aNormal);\n" \
		"vec3 lightDirection = normalize(vec3(uLightPosition - eyeCoordinates));\n" \
		"vec3 ambientLight = uLa * uKa;\n" \
		"vec3 diffuseLight = uLd * uKd * max(dot(lightDirection, transformedNormals), 0.0f);\n" \
		"vec3 reflectionVector = reflect(-lightDirection, transformedNormals);\n" \
		"vec3 viwerVector = normalize(-eyeCoordinates.xyz);\n" \
		"vec3 specularLight = uLs * uKs * pow(max(dot(reflectionVector, viwerVector), 0.0f), uMaterialShininess);\n" \
		"out_Phong_ADS_Light = ambientLight + diffuseLight + specularLight;\n" \
		"}\n" \
		"else\n" \
		"{\n" \
		"out_Phong_ADS_Light = vec3(1.0f, 1.0f, 1.0f);\n" \
		"}\n" \

		"}\n";


	GLuint vertexShaderObject_PV = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShaderObject_PV, 1, (const GLchar**)&vertexShaderSourceCode_PV, NULL);

	glCompileShader(vertexShaderObject_PV);

	GLint status = 0;
	GLint infoLogLength = 0;
	GLchar* szInfoLog = NULL;

	glGetShaderiv(vertexShaderObject_PV, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject_PV, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(vertexShaderObject_PV, infoLogLength, NULL, szInfoLog);

				fprintf(gpFile, "VERTEX SHADER COMPILATION LOG : %s\n\n", szInfoLog);

				free(szInfoLog);
				szInfoLog = NULL;
			}
		}

		uninitialize();
	}


	// FRAGMENT SHADER
	const GLchar* fragmentShaderSourceCode_PV =
		"#version 460 core\n" \
		"in vec3 out_Phong_ADS_Light;\n" \
		"out vec4 FragColor;\n" \
		"void main(void)\n" \
		"{\n" \
		"FragColor = vec4(out_Phong_ADS_Light, 1.0f);\n" \
		"}\n";

	GLuint fragmentShaderObject_PV = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShaderObject_PV, 1, (const GLchar**)&fragmentShaderSourceCode_PV, NULL);

	glCompileShader(fragmentShaderObject_PV);

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(fragmentShaderObject_PV, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject_PV, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(fragmentShaderObject_PV, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "FRAGMENT SHADER COMPILATION LOG : %s\n\n", szInfoLog);

				free(szInfoLog);
				szInfoLog = NULL;
			}
		}

		uninitialize();
	}


	//********************************** PER FRAGMENT *************************************

	//============================================== VERTEX SHADER ====================================================
	const GLchar* vertexShaderSourceCode_PF =
		"#version 460 core\n" \
		"in vec4 aPosition;\n" \
		"in vec3 aNormal;\n" \

		"uniform mat4 uModelMatrix;\n" \
		"uniform mat4 uViewMatrix;\n" \
		"uniform mat4 uProjectionMatrix;\n" \
		"uniform vec4 uLightPosition;\n" \
		"uniform int uLKeyIsPressed;\n" \

		"out vec3 out_transformedNormals;\n" \
		"out vec3 out_lightDirection;\n" \
		"out vec3 out_viwerVector;\n" \

		"void main(void)\n" \
		"{\n" \
			"gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n" \
			
			"if(uLKeyIsPressed == 1)\n" \
			"{\n" \
				"vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition;\n" \
				"mat3 normalMatrix = mat3(uViewMatrix * uModelMatrix);\n" \
				"out_transformedNormals = normalMatrix * aNormal;\n" \
				"out_lightDirection = vec3(uLightPosition - eyeCoordinates);\n" \
				"out_viwerVector = -eyeCoordinates.xyz;\n" \
			"}\n" \
			
		"}\n";


	GLuint vertexShaderObject_PF = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShaderObject_PF, 1, (const GLchar**)&vertexShaderSourceCode_PF, NULL);

	glCompileShader(vertexShaderObject_PF);

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(vertexShaderObject_PF, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject_PF, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(vertexShaderObject_PF, infoLogLength, NULL, szInfoLog);

				fprintf(gpFile, "VERTEX SHADER COMPILATION LOG : %s\n\n", szInfoLog);

				free(szInfoLog);
				szInfoLog = NULL;
			}
		}

		uninitialize();
	}


	// FRAGMENT SHADER
	const GLchar* fragmentShaderSourceCode_PF =
		"#version 460 core\n" \
		"in vec3 out_transformedNormals;\n" \
		"in vec3 out_lightDirection;\n" \
		"in vec3 out_viwerVector;\n" \

		"uniform vec3 uLa;\n" \
		"uniform vec3 uLd;\n" \
		"uniform vec3 uLs;\n" \

		"uniform vec3 uKa;\n" \
		"uniform vec3 uKd;\n" \
		"uniform vec3 uKs;\n" \

		"uniform float uMaterialShininess;\n" \
		"uniform int uLKeyIsPressed;\n" \

		"out vec4 FragColor;\n" \

		"void main(void)\n" \
		"{\n" \
			"vec3 Phong_ADS_Light;\n" \

			"if(uLKeyIsPressed == 1)\n" \
			"{\n" \
				"vec3 normalizedTransformedNormal = normalize(out_transformedNormals);\n" \
				"vec3 normalizedLightDirection = normalize(out_lightDirection);\n" \
				"vec3 normalizedViwerVector = normalize(out_viwerVector);\n" \

				"vec3 ambientLight = uLa * uKa;\n"
			
				"vec3 diffuseLight = uLd * uKd * max(dot(normalizedLightDirection, normalizedTransformedNormal), 0.0f);\n" \

				"vec3 reflectionVector = reflect(-normalizedLightDirection, normalizedTransformedNormal);\n" \

				"vec3 specularLight = uLs * uKs * pow(max(dot(reflectionVector, normalizedViwerVector), 0.0f), uMaterialShininess);\n" \
				
				"Phong_ADS_Light = ambientLight + diffuseLight + specularLight;\n" \

			"}\n" \
			"else"
			"{\n" \
				"Phong_ADS_Light = vec3(1.0f, 1.0f, 1.0f);\n" \
			"}\n" \

			"FragColor = vec4(Phong_ADS_Light, 1.0f);\n" \
		"}\n";

	GLuint fragmentShaderObject_PF = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShaderObject_PF, 1, (const GLchar**)&fragmentShaderSourceCode_PF, NULL);

	glCompileShader(fragmentShaderObject_PF);

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(fragmentShaderObject_PF, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject_PF, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(fragmentShaderObject_PF, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "FRAGMENT SHADER COMPILATION LOG : %s\n\n", szInfoLog);

				free(szInfoLog);
				szInfoLog = NULL;
			}
		}

		uninitialize();
	}



	// CREATE, ATTACH, LINK SHADERPROGRAMOBJECT
	shaderProgramObject_PV = glCreateProgram();
	shaderProgramObject_PF = glCreateProgram();

	glAttachShader(shaderProgramObject_PV, vertexShaderObject_PV);
	glAttachShader(shaderProgramObject_PV, fragmentShaderObject_PV);
	glAttachShader(shaderProgramObject_PF, vertexShaderObject_PF);
	glAttachShader(shaderProgramObject_PF, fragmentShaderObject_PF);

	// Bind shader attribute at a certain index in shader to same index in host program
	glBindAttribLocation(shaderProgramObject_PV, AMC_ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(shaderProgramObject_PV, AMC_ATTRIBUTE_NORMAL, "aNormal");
	glBindAttribLocation(shaderProgramObject_PF, AMC_ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(shaderProgramObject_PF, AMC_ATTRIBUTE_NORMAL, "aNormal");


	// Link PerVertex Shaderprogram Object
	glLinkProgram(shaderProgramObject_PV);
	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;
	glGetProgramiv(shaderProgramObject_PV, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_PV, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject_PV, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "SHADER PROGRAM LINK PERVERTX LOG : %s\n", szInfoLog);

				free(szInfoLog);
				szInfoLog = NULL;
			}
		}

		uninitialize();
	}

	// Link PerFragment Shaderprogram Object
	glLinkProgram(shaderProgramObject_PF);
	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;
	glGetProgramiv(shaderProgramObject_PF, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_PF, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject_PF, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "SHADER PROGRAM LINK PERFRAGMENT LOG : %s\n", szInfoLog);

				free(szInfoLog);
				szInfoLog = NULL;
			}
		}

		uninitialize();
	}


	// Get required the uniform location from the shader
	// Matrix
	modelMatrixUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uModelMatrix");
	viewMatrixUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uViewMatrix");
	projectionMatrixUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uProjectionMatrix");

	// Light
	laUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uLa");
	ldUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uLd");
	lsUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uLs");
	lightPositionUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uLightPosition");

	// Material
	kaUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uKa");
	kdUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uKd");
	ksUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uKs");
	materialShininessUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uMaterialShininess");

	lKeyPressUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uLKeyIsPressed");


	// Get required the uniform location from the shader
	// Matrix
	modelMatrixUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uModelMatrix");
	viewMatrixUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uViewMatrix");
	projectionMatrixUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uProjectionMatrix");

	// Light
	laUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uLa");
	ldUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uLd");
	lsUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uLs");
	lightPositionUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uLightPosition");

	// Material
	kaUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uKa");
	kdUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uKd");
	ksUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uKs");
	materialShininessUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uMaterialShininess");

	lKeyPressUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uLKeyIsPressed");



	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

	// Vertex array object for array's of vertex attribute

	// VAO Bind
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO Position
	glGenBuffers(1, &vbo_position);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// VBO Normal
	glGenBuffers(1, &vbo_normal);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// VBO Element
	glGenBuffers(1, &vbo_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// VAO UnBind
	glBindVertexArray(0);

	// From here onwords OpelGL code start

	// Depth releated code
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LEQUAL);

	// Tell OpenGl to choose the color to clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	perspectiveProjectionMatrix = mat4::identity();   // This is analogs to FFP glLoadIdentity();

	// Warm-up resize
	resize(WIN_WIDTH, WIN_HEIGHT);

	return (0);
}

void printGLInfo(void)
{
	// Variable declaration's
	GLint numExtensions, i;

	// Code
	
	// Print OpenGL information

	fprintf(gpFile, "--- OPENGL INFORMATION ---\n");
	fprintf(gpFile, "*****************************\n\n");

	fprintf(gpFile, "OpenGL Vendor : %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version : %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL Version : %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	// Get number of extensions
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	// Print OpenGL extensions
	for (i = 0; i < numExtensions; ++i)
	{
		fprintf(gpFile, "%d - %s\n", i, glGetStringi(GL_EXTENSIONS, i));
	}

	fprintf(gpFile, "\n*****************************\n");
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

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	// Code

	// Clear OpenGl buufers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use ShaderProgramObject
	if (bPerVertex == TRUE && bPerFragment == FALSE)
	{
		glUseProgram(shaderProgramObject_PV);
	}	
	else if (bPerVertex == FALSE && bPerFragment == TRUE)
	{
		glUseProgram(shaderProgramObject_PF);
	}
	// Transformation's
	mat4 modelMatrix = mat4::identity();   // This analogus to glLoadIdentity() in display, for Modelviewmatrix
	mat4 viewMatrix = mat4::identity();

	mat4 translationMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, -2.0f);

	modelMatrix = translationMatrix; // Order is Importent

	// Send this matrix to the vertex shader in uniform
	if (bPerVertex == TRUE && bPerFragment == FALSE)
	{
		glUniformMatrix4fv(modelMatrixUniform_PV, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniform_PV, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniform_PV, 1, GL_FALSE, perspectiveProjectionMatrix);

		if (bLight == TRUE)
		{
			glUniform3fv(laUniform_PV, 1, lightAmbient);
			glUniform3fv(ldUniform_PV, 1, lightDiffuce);
			glUniform3fv(lsUniform_PV, 1, lightSpeqular);
			glUniform4fv(lightPositionUniform_PV, 1, lightPosition);

			glUniform3fv(kaUniform_PV, 1, materialAmbient);
			glUniform3fv(kdUniform_PV, 1, materialDiffuce);
			glUniform3fv(ksUniform_PV, 1, materialSpeqular);
			glUniform1f(materialShininessUniform_PV, materialShininess);

			glUniform1i(lKeyPressUniform_PV, 1);
		}
		else
		{
			glUniform1i(lKeyPressUniform_PV, 0);
		}
	}
	else if (bPerVertex == FALSE && bPerFragment == TRUE)
	{
		glUniformMatrix4fv(modelMatrixUniform_PF, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniform_PF, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniform_PF, 1, GL_FALSE, perspectiveProjectionMatrix);

		if (bLight == TRUE)
		{
			glUniform3fv(laUniform_PF, 1, lightAmbient);
			glUniform3fv(ldUniform_PF, 1, lightDiffuce);
			glUniform3fv(lsUniform_PF, 1, lightSpeqular);
			glUniform4fv(lightPositionUniform_PF, 1, lightPosition);

			glUniform3fv(kaUniform_PF, 1, materialAmbient);
			glUniform3fv(kdUniform_PF, 1, materialDiffuce);
			glUniform3fv(ksUniform_PF, 1, materialSpeqular);
			glUniform1f(materialShininessUniform_PF, materialShininess);

			glUniform1i(lKeyPressUniform_PF, 1);
		}
		else
		{
			glUniform1i(lKeyPressUniform_PF, 0);
		}

	}

	// Bind with vao
	glBindVertexArray(vao);

	// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	// UnBind vao
	glBindVertexArray(0);

	// UnUse ShaderProgramObject
	glUseProgram(0);

	// Swap the buffers
	SwapBuffers(ghdc);
}

void update(void)
{
	// Code

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
	
	// Free vbo of normal
	if (vbo_element)
	{
		glDeleteBuffers(1, &vbo_element);
		vbo_element = 0;
	}

	// Free vbo of normal
	if (vbo_normal)
	{
		glDeleteBuffers(1, &vbo_normal);
		vbo_normal = 0;
	}

	// Free vbo of position
	if (vbo_position)
	{
		glDeleteBuffers(1, &vbo_position);
		vbo_position = 0;
	}

	// Free vao
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	// Detach, delete ShaderObject and delete shaderProgramObject_PV
	if (shaderProgramObject_PV)
	{
		glUseProgram(shaderProgramObject_PV);

		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject_PV, GL_ATTACHED_SHADERS, &numShaders);
		if (numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject_PV, numShaders, NULL, pShaders);

				for (GLint i = 0; i < numShaders; ++i)
				{
					glDetachShader(shaderProgramObject_PV, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
			}

			free(pShaders);
			pShaders = NULL;
		}

		glUseProgram(0);

		glDeleteProgram(shaderProgramObject_PV);
		shaderProgramObject_PV = 0;
	}

	// Detach, delete ShaderObject and delete shaderProgramObject_PF
	if (shaderProgramObject_PF)
	{
		glUseProgram(shaderProgramObject_PF);

		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject_PF, GL_ATTACHED_SHADERS, &numShaders);
		if (numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject_PF, numShaders, NULL, pShaders);

				for (GLint i = 0; i < numShaders; ++i)
				{
					glDetachShader(shaderProgramObject_PF, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
			}

			free(pShaders);
			pShaders = NULL;
		}

		glUseProgram(0);

		glDeleteProgram(shaderProgramObject_PF);
		shaderProgramObject_PF = 0;
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

