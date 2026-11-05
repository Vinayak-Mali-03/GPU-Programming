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
GLuint shaderProgramObject = 0;

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

BOOL bLight = FALSE;

GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuce[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpeqular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };

GLfloat AngleForXRotation = 0.0f;
GLfloat AngleForYRotation = 0.0f;
GLfloat AngleForZRotation = 0.0f;

GLfloat radius = 100.0f;

GLint Keypressed = -1;

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

	//============================================== VERTEX SHADER ====================================================
	const GLchar* vertexShaderSourceCode =
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
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_NORMAL, "aNormal");

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

	lKeyPressUniform = glGetUniformLocation(shaderProgramObject, "uLKeyIsPressed");

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
	glClearColor(0.50f, 0.50f, 0.50f, 1.0f);

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
	// Local function declaration
	void draw24Spheres(void);

	// Code

	// Clear OpenGl buufers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use ShaderProgramObject
	glUseProgram(shaderProgramObject);

	// Send this matrix to the vertex shader in uniform
	mat4 viewMatrix = mat4::identity();

	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, lightAmbient);
		glUniform3fv(ldUniform, 1, lightDiffuce);
		glUniform3fv(lsUniform, 1, lightSpeqular);

		if (Keypressed == 1)
		{
			lightPosition[0] = 0.0f;
			lightPosition[1] = cos(AngleForXRotation) * radius;
			lightPosition[2] = sin(AngleForXRotation) * radius;
			lightPosition[3] = 1.0f;
		}
		else if (Keypressed == 2)
		{
			lightPosition[0] = cos(AngleForYRotation) * radius;
			lightPosition[1] = 0.0f;
			lightPosition[2] = sin(AngleForYRotation) * radius;
			lightPosition[3] = 1.0f;
		}
		else if (Keypressed == 3)
		{
			lightPosition[0] = cos(AngleForZRotation) * radius;
			lightPosition[1] = sin(AngleForZRotation) * radius;
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform4fv(lightPositionUniform, 1, lightPosition);

		glUniform1i(lKeyPressUniform, 1);

		
		draw24Spheres();
	}
	else
	{
		glUniform1i(lKeyPressUniform, 0);

		draw24Spheres();
	}


	// UnUse ShaderProgramObject
	glUseProgram(0);

	// Swap the buffers
	SwapBuffers(ghdc);
}

void draw24Spheres(void)
{
	// Local function declaration
	void drawSphere(float, float, float, GLfloat*, GLfloat*, GLfloat*, GLfloat);

	// Variable declaration's
	GLfloat materialAmbient[4];
	GLfloat materialDiffuce[4];
	GLfloat materialSpeqular[4];
	GLfloat materialShininess;

	// Code

	// 1st Spheres Of 1st Column  **Emerald**
	materialAmbient[0] = 0.0215f;
	materialAmbient[1] = 0.1745f;
	materialAmbient[2] = 0.0215f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.07568f;
	materialDiffuce[1] = 0.61424f;
	materialDiffuce[2] = 0.07568f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.633f;
	materialSpeqular[1] = 0.727811f;
	materialSpeqular[2] = 0.633f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.6f * 128.0f;

	drawSphere(-2.3f, 2.0f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//********************************************************
	
	// 2nd Spheres Of 1st Column  **Jade**
	materialAmbient[0] = 0.135f;
	materialAmbient[1] = 0.2225f;
	materialAmbient[2] = 0.1575f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.54f;
	materialDiffuce[1] = 0.89f;
	materialDiffuce[2] = 0.63f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.316228f;
	materialSpeqular[1] = 0.316228f;
	materialSpeqular[2] = 0.316228f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.1f * 128.0f;

	drawSphere(-2.3f, 1.2f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//********************************************************

	// 3rd Spheres Of 1st Column  **Obsidian**
	materialAmbient[0] = 0.05375f;
	materialAmbient[1] = 0.05f;
	materialAmbient[2] = 0.06625f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.18275f;
	materialDiffuce[1] = 0.17f;
	materialDiffuce[2] = 0.22525f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.332741f;
	materialSpeqular[1] = 0.328634f;
	materialSpeqular[2] = 0.346435f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.3f * 128.0f;
	
	drawSphere(-2.3f, 0.4f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

	// 4th Spheres Of 1st Column  **Pearl**
	materialAmbient[0] = 0.25f;
	materialAmbient[1] = 0.20725f;
	materialAmbient[2] = 0.20725f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 1.0f;
	materialDiffuce[1] = 0.829f;
	materialDiffuce[2] = 0.829f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.296648f;
	materialSpeqular[1] = 0.296648f;
	materialSpeqular[2] = 0.296648f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.088f * 128.0f;

	drawSphere(-2.3f, -0.4f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

	// 5th Spheres Of 1st Column  **Ruby**
	materialAmbient[0] = 0.1745f;
	materialAmbient[1] = 0.01175f;
	materialAmbient[2] = 0.01175f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.61424f;
	materialDiffuce[1] = 0.04136f;
	materialDiffuce[2] = 0.04136f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.727811f;
	materialSpeqular[1] = 0.626959f;
	materialSpeqular[2] = 0.626959f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.6f * 128.0f;

	drawSphere(-2.3f, -1.2f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

	// 6th Spheres Of 1st Column  **Turquoise**
	materialAmbient[0] = 0.1f;
	materialAmbient[1] = 0.18725f;
	materialAmbient[2] = 0.1745f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.396f;
	materialDiffuce[1] = 0.74151f;
	materialDiffuce[2] = 0.69102f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.297254f;
	materialSpeqular[1] = 0.30829f;
	materialSpeqular[2] = 0.306678f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.1f * 128.0f;

	drawSphere(-2.3f, -2.0f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

	//*********************************************************
	//*********************************************************

	// 1st Spheres Of 2nd Column  **Brass**
	materialAmbient[0] = 0.329412f;
	materialAmbient[1] = 0.223529f;
	materialAmbient[2] = 0.027451;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.780392f;
	materialDiffuce[1] = 0.568627f;
	materialDiffuce[2] = 0.113725f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.992157f;
	materialSpeqular[1] = 0.941176f;
	materialSpeqular[2] = 0.807843f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.21794872f * 128.0f;

	drawSphere(-0.8f, 2.0f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

	// 2nd Spheres Of 2nd Column  **Bronze**
	materialAmbient[0] = 0.2125f;
	materialAmbient[1] = 0.1275f;
	materialAmbient[2] = 0.054f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.714f;
	materialDiffuce[1] = 0.4284f;
	materialDiffuce[2] = 0.18144f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.393548f;
	materialSpeqular[1] = 0.271906f;
	materialSpeqular[2] = 0.166721f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.2f * 128.0f;

	drawSphere(-0.8f, 1.2f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

	// 3rd Spheres Of 2nd Column  **Chrome**
	materialAmbient[0] = 0.25f;
	materialAmbient[1] = 0.25f;
	materialAmbient[2] = 0.25f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.4f;
	materialDiffuce[1] = 0.4f;
	materialDiffuce[2] = 0.4f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.774597f;
	materialSpeqular[1] = 0.774597f;
	materialSpeqular[2] = 0.774597f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.6f * 128.0f;

	drawSphere(-0.8f, 0.4f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

	// 4th Spheres Of 2nd Column  **Copper**
	materialAmbient[0] = 0.19125f;
	materialAmbient[1] = 0.0735f;
	materialAmbient[2] = 0.0225f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.7038f;
	materialDiffuce[1] = 0.27048f;
	materialDiffuce[2] = 0.0828f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.256777f;
	materialSpeqular[1] = 0.137622f;
	materialSpeqular[2] = 0.086014f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.1f * 128.0f;

	drawSphere(-0.8f, -0.4f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

	// 5th Spheres Of 2nd Column  **Gold**
	materialAmbient[0] = 0.24725f;
	materialAmbient[1] = 0.1995f;
	materialAmbient[2] = 0.0745f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.075164f;
	materialDiffuce[1] = 0.60648f;
	materialDiffuce[2] = 0.22648f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.628281f;
	materialSpeqular[1] = 0.555802f;
	materialSpeqular[2] = 0.366065f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.4f * 128.0f;

	drawSphere(-0.8f, -1.2f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

	// 6th Spheres Of 2nd Column  **Silver**
	materialAmbient[0] = 0.19225f;
	materialAmbient[1] = 0.19225f;
	materialAmbient[2] = 0.19225f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.50754f;
	materialDiffuce[1] = 0.50754f;
	materialDiffuce[2] = 0.50754f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.508273f;
	materialSpeqular[1] = 0.508273f;
	materialSpeqular[2] = 0.508273f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.4f * 128.0f;

	drawSphere(-0.8f, -2.0f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

	//*********************************************************
	//*********************************************************

	// 1st Spheres Of 3rd Column  **Black**
	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.01f;
	materialDiffuce[1] = 0.01f;
	materialDiffuce[2] = 0.01f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.50f;
	materialSpeqular[1] = 0.50f;
	materialSpeqular[2] = 0.50f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.25f * 128.0f;

	drawSphere(0.7f, 2.0f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

	// 2nd Spheres Of 3rd Column  **Cyne**
	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.1f;
	materialAmbient[2] = 0.06f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.0f;
	materialDiffuce[1] = 0.50980392f;
	materialDiffuce[2] = 0.50980392f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.50196078f;
	materialSpeqular[1] = 0.50196078f;
	materialSpeqular[2] = 0.50196078f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.25f * 128.0f;

	drawSphere(0.7f, 1.2f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

	// 3rd Spheres Of 3rd Column  **Green**
	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.1f;
	materialDiffuce[1] = 0.35f;
	materialDiffuce[2] = 0.1f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.45f;
	materialSpeqular[1] = 0.55f;
	materialSpeqular[2] = 0.45f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.25f * 128.0f;

	drawSphere(0.7f, 0.4f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

	// 4th Spheres Of 3rd Column  **Red**
	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.5f;
	materialDiffuce[1] = 0.0f;
	materialDiffuce[2] = 0.0f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.7f;
	materialSpeqular[1] = 0.6f;
	materialSpeqular[2] = 0.6f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.25f * 128.0f;

	drawSphere(0.7f, -0.4f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

	// 5th Spheres Of 3rd Column  **White**
	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.55f;
	materialDiffuce[1] = 0.55f;
	materialDiffuce[2] = 0.55f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.70f;
	materialSpeqular[1] = 0.70f;
	materialSpeqular[2] = 0.70f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.25f * 128.0f;

	drawSphere(0.7f, -1.2f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

	// 6th Spheres Of 3rd Column  **Yellow**
	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.5f;
	materialDiffuce[1] = 0.5f;
	materialDiffuce[2] = 0.0f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.60f;
	materialSpeqular[1] = 0.60f;
	materialSpeqular[2] = 0.50f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.25f * 128.0f;

	drawSphere(0.7f, -2.0f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

	//*********************************************************
	//*********************************************************

	// 1st Spheres Of 4th Column  **Black**
	materialAmbient[0] = 0.002f;
	materialAmbient[1] = 0.002f;
	materialAmbient[2] = 0.002f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.01f;
	materialDiffuce[1] = 0.01f;
	materialDiffuce[2] = 0.01f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.4f;
	materialSpeqular[1] = 0.4f;
	materialSpeqular[2] = 0.4f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.078125f * 128.0f;

	drawSphere(2.1f, 2.0f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

	// 2nd Spheres Of 4th Column  **Cyne**
	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.05f;
	materialAmbient[2] = 0.05f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.4f;
	materialDiffuce[1] = 0.5f;
	materialDiffuce[2] = 0.5f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.04f;
	materialSpeqular[1] = 0.7f;
	materialSpeqular[2] = 0.7f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.078125f * 128.0f;

	drawSphere(2.1f, 1.2f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

	// 3rd Spheres Of 4th Column  **Green**
	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.05f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.4f;
	materialDiffuce[1] = 0.5f;
	materialDiffuce[2] = 0.4f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.04f;
	materialSpeqular[1] = 0.7f;
	materialSpeqular[2] = 0.04f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.078125f * 128.0f;

	drawSphere(2.1f, 0.4f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

	// 4th Spheres Of 4th Column  **Red**
	materialAmbient[0] = 0.05f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.5f;
	materialDiffuce[1] = 0.4f;
	materialDiffuce[2] = 0.4f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.7f;
	materialSpeqular[1] = 0.04f;
	materialSpeqular[2] = 0.04f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.078125f * 128.0f;

	drawSphere(2.1f, -0.4f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

	// 5th Spheres Of 4th Column  **White**
	materialAmbient[0] = 0.05f;
	materialAmbient[1] = 0.05f;
	materialAmbient[2] = 0.05f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.5f;
	materialDiffuce[1] = 0.5f;
	materialDiffuce[2] = 0.5f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.7f;
	materialSpeqular[1] = 0.7f;
	materialSpeqular[2] = 0.7f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.078125f * 128.0f;

	drawSphere(2.1f, -1.2f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

	// 6th Spheres Of 4th Column  **Yellow**
	materialAmbient[0] = 0.05f;
	materialAmbient[1] = 0.05f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;

	materialDiffuce[0] = 0.5f;
	materialDiffuce[1] = 0.5f;
	materialDiffuce[2] = 0.4f;
	materialDiffuce[3] = 1.0f;

	materialSpeqular[0] = 0.7f;
	materialSpeqular[1] = 0.7f;
	materialSpeqular[2] = 0.04f;
	materialSpeqular[3] = 1.0f;

	materialShininess = 0.078125f * 128.0f;

	drawSphere(2.1f, -2.0f, -6.0f, materialAmbient, materialDiffuce, materialSpeqular, materialShininess);
	//*********************************************************

}

void drawSphere(float tx, float ty, float tz, GLfloat* pMaterialAmbient, GLfloat* pMaterialDiffuce, GLfloat* pMaterialSpeqular, GLfloat materialShininess)
{
	mat4 modelMatrix = mat4::identity();
	mat4 scaleMatrix = mat4::identity();
	mat4 translationMatrix = mat4::identity();

	// Code

	scaleMatrix = vmath::scale(0.65f, 0.65f, 0.65f);
	translationMatrix = vmath::translate(tx, ty, tz);
	modelMatrix = translationMatrix * scaleMatrix;

	// Send this matrix to the vertex shader in uniform
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

	if (bLight == TRUE)
	{
		glUniform3fv(kaUniform, 1, pMaterialAmbient);
		glUniform3fv(kdUniform, 1, pMaterialDiffuce);
		glUniform3fv(ksUniform, 1, pMaterialSpeqular);
		glUniform1f(materialShininessUniform, materialShininess);
	}

	// Bind with vao
	glBindVertexArray(vao);

	// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	// UnBind vao
	glBindVertexArray(0);
}

void update(void)
{
	// Code

	AngleForXRotation = AngleForXRotation + 0.002f;
	if (AngleForXRotation >= 360.0f)
	{
		AngleForXRotation = AngleForXRotation - 360.0f;
	}

	AngleForYRotation = AngleForYRotation + 0.002f;
	if (AngleForYRotation >= 360.0f)
	{
		AngleForYRotation = AngleForYRotation - 360.0f;
	}

	AngleForZRotation = AngleForZRotation + 0.002f;
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

