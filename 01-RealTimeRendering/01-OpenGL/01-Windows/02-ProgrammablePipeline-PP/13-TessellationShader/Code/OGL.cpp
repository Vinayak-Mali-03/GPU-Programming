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
};

GLuint vao = 0;
GLuint vbo_position = 0;

mat4 perspectiveProjectionMatrix;

GLuint mvpMatrixUniform = 0;

// Tessellation related global variable's
GLuint numberOfLineSegmentsUniform;
GLuint numberOfLineStripsUniform;
GLuint lineColorUniform;

unsigned int uiNumberOfLineSegments = 1;

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
			
			case VK_UP:
				uiNumberOfLineSegments = uiNumberOfLineSegments + 1;
				if (uiNumberOfLineSegments > 30)
				{
					uiNumberOfLineSegments = 30;
				}
				break;
			
			case VK_DOWN:
				uiNumberOfLineSegments = uiNumberOfLineSegments - 1;
				if (uiNumberOfLineSegments <= 0)
				{
					uiNumberOfLineSegments = 1;
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
	//VERTEX SHADER
	const GLchar* vertexShaderSourceCode =
		"#version 460 core\n" \
		"in vec2 aPosition;\n" \
		"void main(void)\n" \
		"{\n" \
			"gl_Position = vec4(aPosition, 0.0f, 1.0f);\n" \
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
	

	//============================================== TESSELLATION CONTROL SHADER ====================================================
	// TESSELLATION CONTROL SHADER                                     // layout == arrangment , patches = order set of vertices
	const GLchar* tessellationControlShaderSourceCode =
		"#version 460 core\n" \
		
		"layout(vertices=4)out;\n" \
		"uniform int uNumberOfLineSegments;\n" \
		"uniform int uNumberOfLineStrips;\n" \

		"void main(void)\n" \
		"{\n" \
			"gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;\n" \
			"gl_TessLevelOuter[0] = float(uNumberOfLineStrips);\n" \
			"gl_TessLevelOuter[1] = float(uNumberOfLineSegments);\n" \
		"}\n";

	GLuint tessellationControlShaderObject = glCreateShader(GL_TESS_CONTROL_SHADER);

	glShaderSource(tessellationControlShaderObject, 1, (const GLchar**)&tessellationControlShaderSourceCode, NULL);

	glCompileShader(tessellationControlShaderObject);

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(tessellationControlShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(tessellationControlShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(tessellationControlShaderObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "TESSELLATION CONTROL SHADER COMPILATION LOG : %s\n\n", szInfoLog);

				free(szInfoLog);
				szInfoLog = NULL;
			}
		}

		uninitialize();
	}


	//============================================== TESSELLATION EVALUATION SHADER ====================================================
// TESSELLATION EVALUTATION SHADER                                     // layout == arrangment
	const GLchar* tessellationEvaluationShaderSourceCode =
		"#version 460 core\n" \

		"layout(isolines)in;\n" \
		"uniform mat4 uMVPMatrix;\n" \

		"void main(void)\n" \
		"{\n" \
			"vec3 p0 = gl_in[0].gl_Position.xyz;\n" \
			"vec3 p1 = gl_in[1].gl_Position.xyz;\n" \
			"vec3 p2 = gl_in[2].gl_Position.xyz;\n" \
			"vec3 p3 = gl_in[3].gl_Position.xyz;\n" \

			"vec3 p = p0 * (1.0f - gl_TessCoord.x) * (1.0f - gl_TessCoord.x) * (1.0f - gl_TessCoord.x) + p1 * 3.0f * gl_TessCoord.x * (1.0f - gl_TessCoord.x) * (1.0f - gl_TessCoord.x) + p2 * 3.0f * gl_TessCoord.x * gl_TessCoord.x * (1.0f - gl_TessCoord.x) + p3 * gl_TessCoord.x * gl_TessCoord.x * gl_TessCoord.x;\n" \

			"gl_Position = uMVPMatrix * vec4(p, 1.0f);\n" \

		"}\n";

	GLuint tessellationEvaluationShaderObject = glCreateShader(GL_TESS_EVALUATION_SHADER);

	glShaderSource(tessellationEvaluationShaderObject, 1, (const GLchar**)&tessellationEvaluationShaderSourceCode, NULL);

	glCompileShader(tessellationEvaluationShaderObject);

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(tessellationEvaluationShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(tessellationEvaluationShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(tessellationEvaluationShaderObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "TESSELLATION EVALUATION SHADER COMPILATION LOG : %s\n\n", szInfoLog);

				free(szInfoLog);
				szInfoLog = NULL;
			}
		}

		uninitialize();
	}


	//============================================== FRAGMENT SHADER ====================================================
	// FRAGMENT SHADER
	const GLchar* fragmentShaderSourceCode =
		"#version 460 core\n" \
		"uniform vec4 uLineColor;\n" \
		"out vec4 FragColor;\n" \

		"void main(void)\n" \
		"{\n" \
			"FragColor = uLineColor;\n" \
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
	glAttachShader(shaderProgramObject, tessellationControlShaderObject);
	glAttachShader(shaderProgramObject, tessellationEvaluationShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	// Bind shader attribute at a certain index in shader to same index in host program
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition");

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
	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "uMVPMatrix");
	numberOfLineSegmentsUniform = glGetUniformLocation(shaderProgramObject, "uNumberOfLineSegments");
	numberOfLineStripsUniform = glGetUniformLocation(shaderProgramObject, "uNumberOfLineStrips");
	lineColorUniform = glGetUniformLocation(shaderProgramObject, "uLineColor");

	// Provide vertex position, color, normal, texcord etc.
	const GLfloat line_position[] = {
										-1.0f, -1.0f,
										-0.5f, 1.0f,
										0.5f, -1.0f,
										1.0f, 1.0f,
									};

	// Vertex array object for array's of vertex attribute
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);

	// Position {6 - Steps for send array} 
 	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_position), line_position, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

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
	glUseProgram(shaderProgramObject);

	// Transformation's
	mat4 modelViewMatrix = mat4::identity();   // This analogus to glLoadIdentity() in display, for Modelviewmatrix

	mat4 translationMatrix = mat4::identity();

	mat4 modelViewProjectionMatrix = mat4::identity();

	translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	modelViewMatrix = translationMatrix; 

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; // Order is importent

	// Send this matrix to the vertex shader in uniform
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	// Send LineSements as uniform
	glUniform1i(numberOfLineSegmentsUniform, uiNumberOfLineSegments);
	
	TCHAR str[255];
	wsprintf(str, TEXT("Vinayak B.Mali : (Number Of Line Segments = %d)"), uiNumberOfLineSegments);
	SetWindowText(ghWnd, str);

	// Send number of strips means number of lines, we have only one
	glUniform1i(numberOfLineStripsUniform, 1);  // As we have only one line

	glUniform4fv(lineColorUniform, 1, vmath::vec4(1.0f, 1.0f, 0.0f, 1.0f));

	// Bind with vao
	glBindVertexArray(vao);

	// Decide how many vertices for one PATCH
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	// Draw the vertex array's
	glDrawArrays(GL_PATCHES, 0, 4);

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

