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
	AMC_ATTRIBUTE_NORMAL,
};

GLuint vao = 0;

GLuint vbo_position = 0;
GLuint vbo_normal = 0;

// Uniform
GLuint modelMatrixUniform = 0;
GLuint viewMatrixUniform = 0;
GLuint projectionMatrixUniform = 0;

// Light
GLuint laUniform[2];
GLuint ldUniform[2];    
GLuint lsUniform[2];
GLuint lightPositionUniform[2];

// Material
GLuint kaUniform = 0;
GLuint kdUniform = 0;
GLuint ksUniform = 0;
GLuint materialShininessUniform = 0;

GLuint lKeyPressUniform = 0;

GLfloat anglePyramid = 0.0f;

mat4 perspectiveProjectionMatrix;

struct Light
{
	vec4 ambient;
	vec4 diffuce;
	vec4 specular;
	vec4 position;
};
struct Light light[2];

GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuce[] = { 0.5f, 0.2f, 0.7f, 1.0f };
GLfloat materialSpeqular[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat materialShininess = 128.0f;

BOOL bLight = FALSE;
BOOL bAnimation = FALSE;

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
				if (bAnimation == TRUE)
				{
					update();
				}
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

		case 'A':
		case 'a':
			if (bAnimation == FALSE)
			{
				bAnimation = TRUE;
			}
			else
			{
				bAnimation = FALSE;
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

		"uniform vec4 uLightPosition[2];\n" \
		"uniform int uLKeyIsPressed;\n" \

		"out vec3 out_transformedNormals;\n" \
		"out vec3 out_lightDirection[2];\n" \
		"out vec3 out_viwerVector;\n" \

		"void main(void)\n" \
		"{\n" \
			"gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n" \
			
			"if(uLKeyIsPressed == 1)\n" \
			"{\n" \
				"vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition;\n" \
				"mat3 normalMatrix = mat3(uViewMatrix * uModelMatrix);\n" \
				"out_transformedNormals = normalMatrix * aNormal;\n" \
				
				"out_lightDirection[0] = vec3(uLightPosition[0] - eyeCoordinates);\n" \
				"out_lightDirection[1] = vec3(uLightPosition[1] -	eyeCoordinates);\n" \
				
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
		"in vec3 out_lightDirection[2];\n" \
		"in vec3 out_viwerVector;\n" \

		"uniform vec3 uLa[2];\n" \
		"uniform vec3 uLd[2];\n" \
		"uniform vec3 uLs[2];\n" \

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

				"vec3 normalizedViwerVector = normalize(out_viwerVector);\n" \


				"vec3 ambientLight[2];\n" \
				"vec3 diffuseLight[2];\n" \
				"vec3 specularLight[2];\n" \
				"vec3 normalizedLightDirection[2];\n" \
				"vec3 reflectionVector[2];\n" \

				"for(int i = 0; i < 2; ++i)\n" \
				"{\n" \
					"normalizedLightDirection[i] = normalize(out_lightDirection[i]);\n" \

					"ambientLight[i] = uLa[i] * uKa;\n"
			
					"diffuseLight[i] = uLd[i] * uKd * max(dot(normalizedLightDirection[i], normalizedTransformedNormal), 0.0f);\n" \

					"reflectionVector[i] = reflect(-normalizedLightDirection[i], normalizedTransformedNormal);\n" \

					"specularLight[i] = uLs[i] * uKs * pow(max(dot(reflectionVector[i], normalizedViwerVector), 0.0f), uMaterialShininess);\n" \
				
					"Phong_ADS_Light = Phong_ADS_Light + ambientLight[i] + diffuseLight[i] + specularLight[i];\n" \
				"}\n" \
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

	// Light 1st
	laUniform[0] = glGetUniformLocation(shaderProgramObject, "uLa[0]");
	ldUniform[0] = glGetUniformLocation(shaderProgramObject, "uLd[0]");
	lsUniform[0] = glGetUniformLocation(shaderProgramObject, "uLs[0]");
	lightPositionUniform[0] = glGetUniformLocation(shaderProgramObject, "uLightPosition[0]");

	// Light 2nd
	laUniform[1] = glGetUniformLocation(shaderProgramObject, "uLa[1]");
	ldUniform[1] = glGetUniformLocation(shaderProgramObject, "uLd[1]");
	lsUniform[1] = glGetUniformLocation(shaderProgramObject, "uLs[1]");
	lightPositionUniform[1] = glGetUniformLocation(shaderProgramObject, "uLightPosition[1]");

	// Material
	kaUniform = glGetUniformLocation(shaderProgramObject, "uKa");
	kdUniform = glGetUniformLocation(shaderProgramObject, "uKd");
	ksUniform = glGetUniformLocation(shaderProgramObject, "uKs");
	materialShininessUniform = glGetUniformLocation(shaderProgramObject, "uMaterialShininess");

	lKeyPressUniform = glGetUniformLocation(shaderProgramObject, "uLKeyIsPressed");

	// Provide vertex position, color, normal, texcord etc.
	const GLfloat pyramid_position[] = {
		// front
		0.0f,  1.0f,  0.0f, // front-top
		-1.0f, -1.0f,  1.0f, // front-left
		1.0f, -1.0f,  1.0f, // front-right

		// right
		0.0f,  1.0f,  0.0f, // right-top
		1.0f, -1.0f,  1.0f, // right-left
		1.0f, -1.0f, -1.0f, // right-right

		// back
		0.0f,  1.0f,  0.0f, // back-top
		1.0f, -1.0f, -1.0f, // back-left
		-1.0f, -1.0f, -1.0f, // back-right

		// left
		0.0f,  1.0f,  0.0f, // left-top
		-1.0f, -1.0f, -1.0f, // left-left
		-1.0f, -1.0f,  1.0f, // left-right
	};

	GLfloat pyramid_normals[] =
	{
		// front
		0.000000f, 0.447214f,  0.894427f, // front-top
		0.000000f, 0.447214f,  0.894427f, // front-left
		0.000000f, 0.447214f,  0.894427f, // front-right

		// right			    
		0.894427f, 0.447214f,  0.000000f, // right-top
		0.894427f, 0.447214f,  0.000000f, // right-left
		0.894427f, 0.447214f,  0.000000f, // right-right

		// back
		0.000000f, 0.447214f, -0.894427f, // back-top
		0.000000f, 0.447214f, -0.894427f, // back-left
		0.000000f, 0.447214f, -0.894427f, // back-right

		// left
	   -0.894427f, 0.447214f,  0.000000f, // left-top
	   -0.894427f, 0.447214f,  0.000000f, // left-left
	   -0.894427f, 0.447214f,  0.000000f, // left-right
	};



	// Vertex array object for array's of vertex attribute

	// VAO Bind
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO Position
	glGenBuffers(1, &vbo_position);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_position), pyramid_position, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// VBO Normal
	glGenBuffers(1, &vbo_normal);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_normals), pyramid_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


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

	// Initialization Of Two Light
	light[0].ambient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light[0].diffuce = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	light[0].specular = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	light[0].position = vec4(-2.0f, 0.0f, 0.0f, 1.0f);

	light[1].ambient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light[1].diffuce = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	light[1].specular = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	light[1].position = vec4(2.0f, 0.0f, 0.0f, 1.0f);

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
	glUseProgram(shaderProgramObject);

	// Transformation's
	mat4 modelMatrix = mat4::identity();   // This analogus to glLoadIdentity() in display, for Modelviewmatrix
	mat4 viewMatrix = mat4::identity();

	mat4 translationMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, -6.0f);

	mat4 rotationMatrix = mat4::identity();
	rotationMatrix = vmath::rotate(anglePyramid, 0.0f, 1.0f, 0.0f);

	modelMatrix = translationMatrix * rotationMatrix; // Order is Importent

	// Send this matrix to the vertex shader in uniform
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	if (bLight == TRUE)
	{
		// Light
		glUniform3fv(laUniform[0], 1, light[0].ambient);
		glUniform3fv(ldUniform[0], 1, light[0].diffuce);
		glUniform3fv(lsUniform[0], 1, light[0].specular);
		glUniform4fv(lightPositionUniform[0], 1, light[0].position);

		glUniform3fv(laUniform[1], 1, light[1].ambient);
		glUniform3fv(ldUniform[1], 1, light[1].diffuce);
		glUniform3fv(lsUniform[1], 1, light[1].specular);
		glUniform4fv(lightPositionUniform[1], 1, light[1].position);

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

	// Bind with vao
	glBindVertexArray(vao);

	// Draw the vertex array's
	glDrawArrays(GL_TRIANGLES, 0, 12);

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
	anglePyramid = anglePyramid + 0.01f;
	if (anglePyramid >= 360.0f)
	{
		anglePyramid = anglePyramid - 360.0f;
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

