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

#define FBO_WIDTH	512
#define FBO_HEIGHT	512

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
	AMC_ATTRIBUTE_TEXCOORD,
	AMC_ATTRIBUTE_NORMAL,
};

GLuint vao_cube = 0;

GLuint vbo_position_cube = 0;
GLuint vbo_texcoord_cube = 0;

GLuint mvpMatrixUniform = 0;

mat4 perspectiveProjectionMatrix;

GLfloat angleCube = 0.0f;

// Variable related with texture
GLuint textureSamplerUniform = 0;

// FBO related global variable's
int winWidth;
int winHeight;

GLuint fbo;
GLuint rbo; 
GLuint fbo_Texture;

int fboResult = -1;

//---------------- Sphere related variable's ------------
GLuint vao_sphere = 0;
GLuint vbo_position_sphere = 0;
GLuint vbo_normal_sphere = 0;
GLuint vbo_element_sphere = 0;

// Uniform
GLuint modelMatrix_SphereUniform = 0;
GLuint viewMatrix_SphereUniform = 0;
GLuint projectionMatrix_SphereUniform = 0;

// Light
GLuint la_SphereUniform = 0;
GLuint ld_SphereUniform = 0;
GLuint ls_SphereUniform = 0;
GLuint lightPosition_SphereUniform = 0;

// Material
GLuint ka_SphereUniform = 0;
GLuint kd_SphereUniform = 0;
GLuint ks_SphereUniform = 0;
GLuint materialShininess_SphereUniform = 0;

GLuint lKeyPress_SphereUniform = 0;

mat4 perspectiveProjectionMatrix_Sphere;

BOOL bLight_Sphere = FALSE;

GLfloat lightAmbient_Sphere[] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat lightDiffuce_Sphere[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpeqular_Sphere[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition_Sphere[] = { 100.0f, 100.0f, 100.0f, 1.0f };

GLfloat materialAmbient_Sphere[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuce_Sphere[] = { 0.5f, 0.2f, 0.7f, 1.0f };
GLfloat materialSpeqular_Sphere[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat materialShininess_Sphere = 128.0f;

// Sphere related variable's
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

unsigned int gNumVertices_Sphere = 0;
unsigned int gNumElements_Sphere = 0;

GLuint shaderProgramObject_Sphere = 0;

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
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDICON));

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
			if (bLight_Sphere == FALSE)
			{
				bLight_Sphere = TRUE;
			}
			else
			{
				bLight_Sphere = FALSE;
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
	BOOL createAndPrepareFBOForDrowing(GLint, GLint);
	int initialize_Sphere(void);

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
		"uniform mat4 uMVPMatrix;\n" \
		"in vec2 aTexCoord;\n" \
		"out vec2 out_TexCoord;\n" \
		"void main(void)\n" \
		"{\n" \
		"out_TexCoord = aTexCoord;\n" \
		"gl_Position = uMVPMatrix * aPosition;\n" \
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
		"in vec2 out_TexCoord;\n" \
		"uniform sampler2D uTextureSampler;\n" \
		"out vec4 FragColor;\n" \
		"void main(void)\n" \
		"{\n" \
		"FragColor = texture(uTextureSampler, out_TexCoord);\n" \
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
	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "uMVPMatrix");

	textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "uTextureSampler");

	// Provide vertex position, color, normal, texcord etc.
	const GLfloat cube_position[] = {
									// front
									1.0f,  1.0f,  1.0f, // top-right of front
									-1.0f,  1.0f,  1.0f, // top-left of front
									-1.0f, -1.0f,  1.0f, // bottom-left of front
									1.0f, -1.0f,  1.0f, // bottom-right of front

									// right
									1.0f,  1.0f, -1.0f, // top-right of right
									1.0f,  1.0f,  1.0f, // top-left of right
									1.0f, -1.0f,  1.0f, // bottom-left of right
									1.0f, -1.0f, -1.0f, // bottom-right of right

									// back
									1.0f,  1.0f, -1.0f, // top-right of back
									-1.0f,  1.0f, -1.0f, // top-left of back
									-1.0f, -1.0f, -1.0f, // bottom-left of back
									1.0f, -1.0f, -1.0f, // bottom-right of back

									// left
									-1.0f,  1.0f,  1.0f, // top-right of left
									-1.0f,  1.0f, -1.0f, // top-left of left
									-1.0f, -1.0f, -1.0f, // bottom-left of left
									-1.0f, -1.0f,  1.0f, // bottom-right of left

									// top
									1.0f,  1.0f, -1.0f, // top-right of top
									-1.0f,  1.0f, -1.0f, // top-left of top
									-1.0f,  1.0f,  1.0f, // bottom-left of top
									1.0f,  1.0f,  1.0f, // bottom-right of top

									// bottom
									1.0f, -1.0f,  1.0f, // top-right of bottom
									-1.0f, -1.0f,  1.0f, // top-left of bottom
									-1.0f, -1.0f, -1.0f, // bottom-left of bottom
									1.0f, -1.0f, -1.0f, // bottom-right of bottom
	};
	const GLfloat cube_texcoord[] = {
								// front
								1.0f, 1.0f, // top-right of front
								0.0f, 1.0f, // top-left of front
								0.0f, 0.0f, // bottom-left of front
								1.0f, 0.0f, // bottom-right of front

								// right
								1.0f, 1.0f, // top-right of right
								0.0f, 1.0f, // top-left of right
								0.0f, 0.0f, // bottom-left of right
								1.0f, 0.0f, // bottom-right of right

								// back
								1.0f, 1.0f, // top-right of back
								0.0f, 1.0f, // top-left of back
								0.0f, 0.0f, // bottom-left of back
								1.0f, 0.0f, // bottom-right of back

								// left
								1.0f, 1.0f, // top-right of left
								0.0f, 1.0f, // top-left of left
								0.0f, 0.0f, // bottom-left of left
								1.0f, 0.0f, // bottom-right of left

								// top
								1.0f, 1.0f, // top-right of top
								0.0f, 1.0f, // top-left of top
								0.0f, 0.0f, // bottom-left of top
								1.0f, 0.0f, // bottom-right of top

								// bottom
								1.0f, 1.0f, // top-right of bottom
								0.0f, 1.0f, // top-left of bottom
								0.0f, 0.0f, // bottom-left of bottom
								1.0f, 0.0f, // bottom-right of bottom
							};

	// RECTANGLE
	// Vertex array object for array's of vertex attribute
	glGenVertexArrays(1, &vao_cube);

	// VAO bind
	glBindVertexArray(vao_cube);
	//----------------------------------------------------------

	// Position {6 - Steps for send array} 
	glGenBuffers(1, &vbo_position_cube);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_cube);

	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_position), cube_position, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// TEXCOORD

	// Texture {6 - Steps for send array} 
	glGenBuffers(1, &vbo_texcoord_cube);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord_cube);

	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_texcoord), cube_texcoord, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);

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
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_TEXTURE_2D);

	perspectiveProjectionMatrix = mat4::identity();   // This is analogs to FFP glLoadIdentity();

	// Warm-up resize
	resize(WIN_WIDTH, WIN_HEIGHT);

	// Create FBO and if successfull call initialize Sphere
	if (createAndPrepareFBOForDrowing(FBO_WIDTH, FBO_HEIGHT) == TRUE)
	{
		fprintf(gpFile, "FBO Creation Successfull !!!\n");

		fboResult = initialize_Sphere();
		if (fboResult != 0)
		{
			fprintf(gpFile, "ERROR : initialize_Sphere() Failed !!!\n");

			return(-7);
		}
		else
		{
			fprintf(gpFile, "initialzie_Sphere() Successfull !!!\n");
		}
	}
	else
	{
		fprintf(gpFile, "FBO Creation Failed !!!\n");
		return(-8);
	}

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

BOOL createAndPrepareFBOForDrowing(GLint textureWidth, GLint textureHeight)
{
	// Variable declaration's
	
	// Code

	// Setp-1
	// Check wether texture width and texture height parameter's are compatible
	GLint maxRenderBufferSize;
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize);
	if (maxRenderBufferSize <= textureWidth || maxRenderBufferSize <= textureHeight)
	{
		fprintf(gpFile, "FBOWidth Or FBOHeight are exiding maxRenderBufferSize !!!\n");
		return(FALSE);
	}

	// Setp-2
	// Create new Framebuffer for our sphere
	glGenFramebuffers(1, &fbo);

	// Setp-3
	// Bind with the newly created Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Setp-4
	// Create render buffer as placeholder for out depth buffer for our frame buffer
	glGenRenderbuffers(1, &rbo);

	// Setp-5
	// Bind with this newly created render buffer
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	// Step-6
	// Give specific storage to this render buffer so depth needed for sphere will reside here
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, textureWidth, textureHeight);

	// Step-7
	// Create empty but fully capable and compatible texture object for incoming sphere
	glGenTextures(1, &fbo_Texture);
	
	glBindTexture(GL_TEXTURE_2D, fbo_Texture);

	// Step-8
	// To create full feature texture give appropriate parameter's to textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	// Step-9
	// Now create this texture for our FBO
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL); // Last "NULL" indicate's that this is empty texture

	// Step-10
	// Genrate texture mipmaps
	glGenerateMipmap(GL_TEXTURE_2D);

	// Step-11
	// Give above empty texture to our frmebuffer 
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_Texture, 0);

	// Setp-12
	// Attach priously created depth render buffer to our frame buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// Step-13
	// Check wether our framebuffer is complete or not
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(gpFile, "FBO Creation is incomplete !!!\n");

		return (FALSE);
	}

	// Step-14
	// Unbind with newly create framebuffer we will use it in future when needed
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return(TRUE);
}

int initialize_Sphere(void)
{
	// Local variable declaration's
	void resize_Sphere(int, int);
	void uninitialize_Sphere(void);

	// Code

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

		uninitialize_Sphere();
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

		uninitialize_Sphere();
	}

	// CREATE, ATTACH, LINK SHADERPROGRAMOBJECT
	shaderProgramObject_Sphere = glCreateProgram();

	glAttachShader(shaderProgramObject_Sphere, vertexShaderObject);
	glAttachShader(shaderProgramObject_Sphere, fragmentShaderObject);

	// Bind shader attribute at a certain index in shader to same index in host program
	glBindAttribLocation(shaderProgramObject_Sphere, AMC_ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(shaderProgramObject_Sphere, AMC_ATTRIBUTE_NORMAL, "aNormal");

	glLinkProgram(shaderProgramObject_Sphere);

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(shaderProgramObject_Sphere, GL_LINK_STATUS, &status);
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

		uninitialize_Sphere();
	}

	// Get required the uniform location from the shader
	// Matrix
	modelMatrix_SphereUniform = glGetUniformLocation(shaderProgramObject_Sphere, "uModelMatrix");
	viewMatrix_SphereUniform = glGetUniformLocation(shaderProgramObject_Sphere, "uViewMatrix");
	projectionMatrix_SphereUniform = glGetUniformLocation(shaderProgramObject_Sphere, "uProjectionMatrix");

	// Light
	la_SphereUniform = glGetUniformLocation(shaderProgramObject_Sphere, "uLa");
	ld_SphereUniform = glGetUniformLocation(shaderProgramObject_Sphere, "uLd");
	ls_SphereUniform = glGetUniformLocation(shaderProgramObject_Sphere, "uLs");
	lightPosition_SphereUniform = glGetUniformLocation(shaderProgramObject_Sphere, "uLightPosition");

	// Material
	ka_SphereUniform = glGetUniformLocation(shaderProgramObject_Sphere, "uKa");
	kd_SphereUniform = glGetUniformLocation(shaderProgramObject_Sphere, "uKd");
	ks_SphereUniform = glGetUniformLocation(shaderProgramObject_Sphere, "uKs");
	materialShininess_SphereUniform = glGetUniformLocation(shaderProgramObject_Sphere, "uMaterialShininess");

	lKeyPress_SphereUniform = glGetUniformLocation(shaderProgramObject_Sphere, "uLKeyIsPressed");

	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices_Sphere = getNumberOfSphereVertices();
	gNumElements_Sphere = getNumberOfSphereElements();

	// Vertex array object for array's of vertex attribute

	// VAO Bind
	glGenVertexArrays(1, &vao_sphere);
	glBindVertexArray(vao_sphere);

	// VBO Position
	glGenBuffers(1, &vbo_position_sphere);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// VBO Normal
	glGenBuffers(1, &vbo_normal_sphere);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// VBO Element
	glGenBuffers(1, &vbo_element_sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere);
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

	perspectiveProjectionMatrix_Sphere = mat4::identity();   // This is analogs to FFP glLoadIdentity();

	// Warm-up resize
	resize_Sphere(FBO_WIDTH, FBO_HEIGHT);

	return (0);
}

void resize(int width, int height)
{
	// Code

	// If height by accsident is less than 0 then height should be 1
	if (height <= 0)
	{
		height = 1;
	}

	winWidth = width;
	winHeight = height;

	// Set the viewport
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void resize_Sphere(int width, int height)
{
	// Code

	// If height by accsident is less than 0 then height should be 1
	if (height <= 0)
	{
		height = 1;
	}

	// Set the viewport
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix_Sphere = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	// Local function declaration's
	void resize(int, int);
	void display_Sphere(void);

	// Code

	// Call sphere related code
	if (fboResult == 0)  // Means initialize_Sphere is successfully executed in initialize()
	{
		display_Sphere();
	}

	// Call cube's resize to compensete sphere's resize
	resize(winWidth, winHeight);

	// ClearColor again to compensete the effect of ClearColor of sphere
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Clear OpenGl buufers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use ShaderProgramObject
	glUseProgram(shaderProgramObject);

	// CUBE
	// Transformation's
	mat4 modelViewMatrix = mat4::identity();   // This analogus to glLoadIdentity() in display, for Modelviewmatrix

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

	modelViewMatrix = translationMatrix * rotationMatrix; // Order is Importent

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; // Order is importent

	// Send this matrix to the vertex shader in uniform
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//For Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbo_Texture);
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

	glBindTexture(GL_TEXTURE_2D, 0);

	// Unbind VAO
	glBindVertexArray(0);

	// UnUse ShaderProgramObject
	glUseProgram(0);

	// Swap the buffers
	SwapBuffers(ghdc);
}

void display_Sphere(void)
{
	// Local function declaration's
	void resize_Sphere(int, int);

	// Code

	// Do FBO Related Changes
	if (fbo)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}

	// Call sphere reisze to compensente the effect of cube resize
	resize_Sphere(FBO_WIDTH, FBO_HEIGHT);

	// Call ClearColor of sphere to compensente the effect of ClearColor of cube
	glClearColor(0.0f, 0.0f, 0.0f, 1.0);

	// Clear OpenGl buufers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use ShaderProgramObject
	glUseProgram(shaderProgramObject_Sphere);

	// Transformation's
	mat4 modelMatrix = mat4::identity();   // This analogus to glLoadIdentity() in display, for Modelviewmatrix
	mat4 viewMatrix = mat4::identity();

	mat4 translationMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, -2.0f);

	modelMatrix = translationMatrix; // Order is Importent

	// Send this matrix to the vertex shader in uniform
	glUniformMatrix4fv(modelMatrix_SphereUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_SphereUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrix_SphereUniform, 1, GL_FALSE, perspectiveProjectionMatrix_Sphere);

	if (bLight_Sphere == TRUE)
	{
		glUniform3fv(la_SphereUniform, 1, lightAmbient_Sphere);
		glUniform3fv(ld_SphereUniform, 1, lightDiffuce_Sphere);
		glUniform3fv(ls_SphereUniform, 1, lightSpeqular_Sphere);
		glUniform4fv(lightPosition_SphereUniform, 1, lightPosition_Sphere);

		glUniform3fv(ka_SphereUniform, 1, materialAmbient_Sphere);
		glUniform3fv(kd_SphereUniform, 1, materialDiffuce_Sphere);
		glUniform3fv(ks_SphereUniform, 1, materialSpeqular_Sphere);
		glUniform1f(materialShininess_SphereUniform, materialShininess_Sphere);

		glUniform1i(lKeyPress_SphereUniform, 1);
	}
	else
	{
		glUniform1i(lKeyPress_SphereUniform, 0);
	}

	// Bind with vao
	glBindVertexArray(vao_sphere);

	// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere);
	glDrawElements(GL_TRIANGLES, gNumElements_Sphere, GL_UNSIGNED_SHORT, 0);

	// UnBind vao
	glBindVertexArray(0);

	// UnUse ShaderProgramObject
	glUseProgram(0);

	if (fbo)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
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
	void uninitialize_Sphere(void);

	// Code

	if (fboResult == 0)  // Means sphere initialize() is successfuly executed
	{
		uninitialize_Sphere();
	}

	// If user is exiting in full screen then restore the full screen back to normal
	if (gbFullScreen == TRUE)
	{
		toggleFullScreen();

		gbFullScreen = FALSE;
	}

	// Dlete FBO Texture
	if (fbo_Texture)
	{
		glDeleteTextures(1, &fbo_Texture);
		fbo_Texture = 0;
	}

	if (rbo)
	{
		glDeleteRenderbuffers(1, &rbo);
		rbo = 0;
	}

	if (fbo)
	{
		glDeleteFramebuffers(1, &fbo);
		fbo = 0;
	}

	// Free vbo of color
	if (vbo_texcoord_cube)
	{
		glDeleteBuffers(1, &vbo_texcoord_cube);
		vbo_texcoord_cube = 0;
	}

	// Free vbo of position
	if (vbo_position_cube)
	{
		glDeleteBuffers(1, &vbo_position_cube);
		vbo_position_cube = 0;
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

void uninitialize_Sphere(void)
{
	// Code

	// Free vbo of normal
	if (vbo_element_sphere)
	{
		glDeleteBuffers(1, &vbo_element_sphere);
		vbo_element_sphere = 0;
	}

	// Free vbo of normal
	if (vbo_normal_sphere)
	{
		glDeleteBuffers(1, &vbo_normal_sphere);
		vbo_normal_sphere = 0;
	}

	// Free vbo of position
	if (vbo_position_sphere)
	{
		glDeleteBuffers(1, &vbo_position_sphere);
		vbo_position_sphere = 0;
	}

	// Free vao
	if (vao_sphere)
	{
		glDeleteVertexArrays(1, &vao_sphere);
		vao_sphere = 0;
	}

	// Detach, delete ShaderObject and delete shaderProgramObject
	if (shaderProgramObject_Sphere)
	{
		glUseProgram(shaderProgramObject_Sphere);

		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject_Sphere, GL_ATTACHED_SHADERS, &numShaders);
		if (numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject_Sphere, numShaders, NULL, pShaders);

				for (GLint i = 0; i < numShaders; ++i)
				{
					glDetachShader(shaderProgramObject_Sphere, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
			}

			free(pShaders);
			pShaders = NULL;
		}

		glUseProgram(0);

		glDeleteProgram(shaderProgramObject_Sphere);
		shaderProgramObject_Sphere = 0;
	}

}

