// Win32 / Standard header file's
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

// OpgnGl related header file's
#include <GL/glew.h>     // This header file must be included before "gl.h"
#include <gl/GL.h>
#include "vmath.h"		// matrix related and transformation related matrix

using namespace vmath;

// Custom header file's
#include "OGL.h"

// OpglGl related librarie's
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

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
GLuint shaderProgramObject = 0;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD,
};

GLuint vao_cube = 0;

GLuint vbo_cube = 0;

// Uniform
GLuint modelMatrixUniform = 0;
GLuint viewMatrixUniform = 0;
GLuint projectionMatrixUniform = 0;

// Light
GLuint laUniform = 0;
GLuint ldUniform = 0;
GLuint lsUniform = 0;
GLuint lightPositionUniform = 0;

// Material
GLuint kaUniform = 0;
GLuint kdUniform = 0;
GLuint ksUniform = 0;
GLuint materialShininessUniform = 0;

GLuint lKeyPressUniform = 0;

mat4 perspectiveProjectionMatrix;

GLfloat angleCube = 0.0f;

// Variable related with texture
GLuint texture_Marble = 0;
GLuint textureSamplerUniform = 0;

BOOL bLight = FALSE;

GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuce[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpeqular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };

GLfloat materialAmbient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat materialDiffuce[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpeqular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialShininess = 128.0f;


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
	BOOL loadGLTexture(GLuint*, TCHAR[]);

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

	//============================================== VERTEX SHADER ====================================================
	const GLchar* vertexShaderSourceCode =
		"#version 460 core\n" \
		
		"in vec4 aPosition;\n" \
		"in vec4 aColor;\n" \
		"in vec3 aNormal;\n" \
		"in vec2 aTexCoord;\n" \

		"uniform mat4 uModelMatrix;\n" \
		"uniform mat4 uViewMatrix;\n" \
		"uniform mat4 uProjectionMatrix;\n" \
		"uniform vec4 uLightPosition;\n" \
		"uniform int uLKeyIsPressed;\n" \

		"out vec4 out_color;\n" \
		"out vec3 out_transformedNormals;\n" \
		"out vec3 out_lightDirection;\n" \
		"out vec3 out_viwerVector;\n" \
		"out vec2 out_TexCoord;\n" \

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

			"out_color = aColor;\n"\
			"out_TexCoord = aTexCoord;\n" \
		"}\n";

	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	glCompileShader(vertexShaderObject);

	GLint status = 0;
	GLint infoLogLength = 0;
	GLchar* szInfoLog = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, NULL, szInfoLog);

				fprintf(gpFile, "VERTEX SHADER COMPILATION LOG : %s\n\n", szInfoLog);

				free(szInfoLog);
				szInfoLog = NULL;
			}
		}

		uninitialize();
	}


	// FRAGMENT SHADER
	const GLchar* fragmentShaderSourceCode =
		"#version 460 core\n" \

		"in vec4 out_color;\n" \
		"in vec3 out_transformedNormals;\n" \
		"in vec3 out_lightDirection;\n" \
		"in vec3 out_viwerVector;\n" \
		"in vec2 out_TexCoord;\n" \

		"uniform vec3 uLa;\n" \
		"uniform vec3 uLd;\n" \
		"uniform vec3 uLs;\n" \

		"uniform vec3 uKa;\n" \
		"uniform vec3 uKd;\n" \
		"uniform vec3 uKs;\n" \

		"uniform float uMaterialShininess;\n" \
		"uniform int uLKeyIsPressed;\n" \

		"uniform sampler2D uTextureSampler;\n" \

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

			"vec4 tex = texture(uTextureSampler, out_TexCoord);\n" \

			"FragColor = out_color * tex * vec4(Phong_ADS_Light, 1.0f);\n" \
		"}\n";

	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	glCompileShader(fragmentShaderObject);

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "FRAGMENT SHADER COMPILATION LOG : %s\n\n", szInfoLog);

				free(szInfoLog);
				szInfoLog = NULL;
			}
		}

		uninitialize();
	}

	// CREATE, ATTACH, LINK SHADERPROGRAMOBJECT
	shaderProgramObject = glCreateProgram();

	glAttachShader(shaderProgramObject, vertexShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	// Bind shader attribute at a certain index in shader to same index in host program
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition");

	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_COLOR, "aColor");

	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_NORMAL, "aNormal");

	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_TEXCOORD, "aTexCoord");

	glLinkProgram(shaderProgramObject);

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "SHADER PROGRAM LINK LOG : %s\n", szInfoLog);

				free(szInfoLog);
				szInfoLog = NULL;
			}
		}

		uninitialize();
	}

	// Get required the uniform location from the shader
	// Matrix
	modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "uModelMatrix");
	viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "uViewMatrix");
	projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "uProjectionMatrix");

	// Light
	laUniform = glGetUniformLocation(shaderProgramObject, "uLa");
	ldUniform = glGetUniformLocation(shaderProgramObject, "uLd");
	lsUniform = glGetUniformLocation(shaderProgramObject, "uLs");
	lightPositionUniform = glGetUniformLocation(shaderProgramObject, "uLightPosition");

	// Material
	kaUniform = glGetUniformLocation(shaderProgramObject, "uKa");
	kdUniform = glGetUniformLocation(shaderProgramObject, "uKd");
	ksUniform = glGetUniformLocation(shaderProgramObject, "uKs");
	materialShininessUniform = glGetUniformLocation(shaderProgramObject, "uMaterialShininess");

	textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "uTextureSampler");

	lKeyPressUniform = glGetUniformLocation(shaderProgramObject, "uLKeyIsPressed");

	// Provide vertex position, color, normal, texcord etc.
	const GLfloat cube_PCNT[] =
	{
		// front
		// position				// color			 // normals				// texcoords
		 1.0f,  1.0f,  1.0f,	1.0f, 0.0f, 0.0f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,	1.0f, 0.0f, 0.0f,	 0.0f,  0.0f,  1.0f,	0.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,	1.0f, 0.0f, 0.0f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f,	1.0f, 0.0f, 0.0f,	 0.0f,  0.0f,  1.0f,	1.0f, 0.0f,

		 // right			 
		 // position				// color			 // normals				// texcoords
		  1.0f,  1.0f, -1.0f,	0.0f, 0.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
		  1.0f,  1.0f,  1.0f,	0.0f, 0.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		  1.0f, -1.0f,  1.0f,	0.0f, 0.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
		  1.0f, -1.0f, -1.0f,	0.0f, 0.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	1.0f, 0.0f,

		  // back				 
		  // position				// color			 // normals				// texcoords
		   1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 0.0f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
		  -1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 0.0f,	 0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
		  -1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 0.0f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
		   1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 0.0f,	 0.0f,  0.0f, -1.0f,	1.0f, 0.0f,

		   // left				 
		   // position				// color			 // normals				// texcoords
		   -1.0f,  1.0f,  1.0f,	1.0f, 0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
		   -1.0f,  1.0f, -1.0f,	1.0f, 0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		   -1.0f, -1.0f, -1.0f,	1.0f, 0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
		   -1.0f, -1.0f,  1.0f,	1.0f, 0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,

		   // top				 
		   // position				// color			 // normals				// texcoords
			1.0f,  1.0f, -1.0f,	0.0f, 1.0f, 0.0f,	 0.0f,  1.0f,  0.0f,	1.0f, 1.0f,
		   -1.0f,  1.0f, -1.0f,	0.0f, 1.0f, 0.0f,	 0.0f,  1.0f,  0.0f,	0.0f, 1.0f,
		   -1.0f,  1.0f,  1.0f,	0.0f, 1.0f, 0.0f,	 0.0f,  1.0f,  0.0f,	0.0f, 0.0f,
			1.0f,  1.0f,  1.0f,	0.0f, 1.0f, 0.0f,	 0.0f,  1.0f,  0.0f,	1.0f, 0.0f,

			// bottom			 
			// position				// color			 // normals				// texcoords
			 1.0f, -1.0f,  1.0f,	1.0f, 0.5f, 0.0f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
			-1.0f, -1.0f,  1.0f,	1.0f, 0.5f, 0.0f,	 0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
			-1.0f, -1.0f, -1.0f,	1.0f, 0.5f, 0.0f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,
			 1.0f, -1.0f, -1.0f,	1.0f, 0.5f, 0.0f,	 0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
	};

	// CUBE
	// Vertex array object for array's of vertex attribute
	glGenVertexArrays(1, &vao_cube);

	// VAO bind
	glBindVertexArray(vao_cube);
	//----------------------------------------------------------

	// Create a common vbo for P, C, N, T.
	glGenBuffers(1, &vbo_cube);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube);

	// sizeof(cube_CPNT) == (24 * 11 * 4)  FOR ANDROID
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_PCNT), cube_PCNT, GL_STATIC_DRAW);
	// For Position
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GL_FLOAT), (void*)(0 * sizeof(GL_FLOAT)));   // Stride (udi/dhang)  == 11
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	
	// For Color
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	
	// For Normal
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GL_FLOAT), (void*)(6 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	
	// For TextCoord
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GL_FLOAT), (void*)(9 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//----------------------------------------------------------
	// VAO unBind
	glBindVertexArray(0);

	// From here onwords OpelGL code start

	// Depth releated code
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LEQUAL);

	// Tell OpenGl to choose the color to clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Load textures
	if (loadGLTexture(&texture_Marble, MAKEINTRESOURCE(IDBITMAP_MARBLE)) == FALSE)
	{
		fprintf(gpFile, "ERROR : loadGLTexture() failed to create marble_bmp...!\n");
		return(-8);
	}

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

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);

		glGenerateMipmap(GL_TEXTURE_2D);

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

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	// Code

	// Clear OpenGl buufers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use ShaderProgramObject
	glUseProgram(shaderProgramObject);

	// CUBE
	// Transformation's
	mat4 modelMatrix = mat4::identity();   // This analogus to glLoadIdentity() in display, for Modelviewmatrix
	mat4 viewMatrix = mat4::identity();

	mat4 translationMatrix = mat4::identity();

	mat4 modelViewProjectionMatrix = mat4::identity();

	translationMatrix = vmath::translate(0.0f, 0.0f, -6.0f);

	mat4 rotationMatrix = mat4::identity();

	mat4 rotationMatrixX = mat4::identity();
	rotationMatrixX = vmath::rotate(angleCube, 1.0f, 0.0f, 0.0f);

	mat4 rotationMatrixY = mat4::identity();
	rotationMatrixY = vmath::rotate(angleCube, 0.0f, 1.0f, 0.0f);

	mat4 rotationMatrixZ = mat4::identity();
	rotationMatrixZ = vmath::rotate(angleCube, 0.0f, 0.0f, 1.0f);

	rotationMatrix = rotationMatrixX * rotationMatrixY * rotationMatrixZ;

	modelMatrix = translationMatrix * rotationMatrix; // Order is Importent

	// Send this matrix to the vertex shader in uniform
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, lightAmbient);
		glUniform3fv(ldUniform, 1, lightDiffuce);
		glUniform3fv(lsUniform, 1, lightSpeqular);
		glUniform4fv(lightPositionUniform, 1, lightPosition);

		glUniform3fv(kaUniform, 1, materialAmbient);
		glUniform3fv(kdUniform, 1, materialDiffuce);
		glUniform3fv(ksUniform, 1, materialSpeqular);
		glUniform1f(materialShininessUniform, materialShininess);

		glUniform1i(lKeyPressUniform, 1);
	}
	else
	{
		glUniform1i(lKeyPressUniform, 0);
	}

	//For Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_Marble);
	glUniform1i(textureSamplerUniform, 0);

	// Bind with vao
	glBindVertexArray(vao_cube);

	// Draw the vertex array's
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glBindVertexArray(0);

	// UnUse ShaderProgramObject
	glUseProgram(0);

	// Swap the buffers
	SwapBuffers(ghdc);
}

void update(void)
{
	// Code

	//CUBE
	angleCube = angleCube - 0.01f;
	if (angleCube <= 0.0f)
	{
		angleCube = angleCube + 360.0f;
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

	// Free texture
	if (texture_Marble)
	{
		glDeleteTextures(1, &texture_Marble);
		texture_Marble = 0;
	}

	// Free vbo of position
	if (vbo_cube)
	{
		glDeleteBuffers(1, &vbo_cube);
		vbo_cube = 0;
	}

	// Free vao
	if (vao_cube)
	{
		glDeleteVertexArrays(1, &vao_cube);
		vao_cube = 0;
	}


	// Detach, delete ShaderObject and delete shaderProgramObject
	if (shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);

		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numShaders);
		if (numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject, numShaders, NULL, pShaders);

				for (GLint i = 0; i < numShaders; ++i)
				{
					glDetachShader(shaderProgramObject, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
			}

			free(pShaders);
			pShaders = NULL;
		}

		glUseProgram(0);

		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
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

