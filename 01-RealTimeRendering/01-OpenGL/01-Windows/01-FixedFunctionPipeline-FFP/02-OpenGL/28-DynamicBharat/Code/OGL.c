// Win32 / Standard header file's
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// OpenGL releated header file's
#include <gl/GL.h>
#include <gl/GLU.h>

// Custom header file's
#include "OGL.h"

// OpenGL releated librarie's
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "GLU32.lib")

// For playsound
#pragma comment(lib, "winmm.lib")

// Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define PI 3.1415

// Gloabal callback declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global variable declaration's

// Active window releated variable's
BOOL gbActiveWindow = FALSE;

// Exit key press releated variable's
BOOL gbEscapeKeyIsPressed = FALSE;

// Variable's releated with full screen
BOOL gbFullScreen = FALSE;
HWND ghWnd = NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev;

// Variable's releated with file I/O
char gszLogFileName[] = "Log.txt";
FILE *gpFile = NULL;

// OpgnGL related global variable's
HDC ghdc = NULL;
HGLRC ghrc = NULL;

// Variable's releated with plane
GLfloat X1 = -3.f;
GLfloat Y1 = -1.0f;

GLfloat X2 = -3.0f;
GLfloat Y2 = 1.0f;

GLfloat X3 = -3.0f;
GLfloat Y3 = 0.0f;

// Variable's releated with letters
BOOL bDoneLetters = FALSE;

int iLetterNumber = 1;

// For letter 'B'
GLfloat XB = -2.5f;

// For letter 'H'
GLfloat XH = -2.5f;
GLfloat YH = 2.5f;

// For letter 'A' (First)
GLfloat XA1 = -2.5f;
GLfloat YA1 = -2.5f;

// For letter 'R'
GLfloat XR = 2.5f;
GLfloat YR = 2.5f;

// Fort letter 'A' (Second)
GLfloat XA2 = 2.5f;
GLfloat YA2 = -2.5f;

// For letter 'T'
GLfloat XT = 2.5f;

// Variable's releated color bars
BOOL bB = FALSE;
BOOL bH = FALSE;
BOOL bA1 = FALSE;
BOOL bR = FALSE;
BOOL bA2 = FALSE;
BOOL bT = FALSE;

BOOL Bpass = FALSE;
BOOL Hpass = FALSE;
BOOL A1pass = FALSE;
BOOL Rpass = FALSE;
BOOL A2pass = FALSE;
BOOL Tpass = FALSE;

float t = 0.3f;
BOOL tail = TRUE;

// Entry-point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iShowCmd)
{
    // Local function declaration's
    int initialize(void);
    void display(void);
    void update(void);
    void uninitialize(void);

    // Variable declaration's
    WNDCLASSEX wnd;
    HWND hWnd = NULL;
    MSG msg;
    TCHAR szClassName[] = TEXT("RTR-06");
    BOOL bDone = FALSE;

    // Code

    // Create log file
    gpFile = fopen(gszLogFileName, "w");
    if (gpFile == NULL)
    {
        MessageBox(NULL, TEXT("Log file creation failed...!"), TEXT("File Error"), MB_OK | MB_ICONERROR);
        exit(0);
    }
    else
    {
        fprintf(gpFile, "Program started successfully...!\n\n");
    }

    // Window class initialization
    wnd.cbSize = sizeof(WNDCLASSEX);
    wnd.cbClsExtra = 0;
    wnd.cbWndExtra = 0;
    wnd.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
    wnd.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
    wnd.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
    wnd.hInstance = hInstance;
    wnd.lpfnWndProc = WndProc;
    wnd.lpszClassName = szClassName;
    wnd.lpszMenuName = NULL;
    wnd.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

    // Registeration of window class
    if (!RegisterClassEx(&wnd))
    {
        MessageBox(NULL, TEXT("Failed to register window class"), TEXT("RegisterClassEx"), MB_OK | MB_ICONERROR);
        ExitProcess(EXIT_FAILURE);
    }

    // Create window
    hWnd = CreateWindowEx(WS_EX_APPWINDOW,
                          szClassName,
                          TEXT("Vinayak B. Mali"),
                          WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
                          (GetSystemMetrics(SM_CXSCREEN) / 2) - (WIN_WIDTH / 2),
                          (GetSystemMetrics(SM_CYSCREEN) / 2) - (WIN_HEIGHT / 2),
                          WIN_WIDTH,
                          WIN_HEIGHT,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    if (hWnd == NULL)
    {
        MessageBox(NULL, TEXT("Failed to create window"), TEXT("CreateWindowEx"), MB_OK | MB_ICONERROR);
        ExitProcess(EXIT_FAILURE);
    }

    ghWnd = hWnd;

    // ShowWindow
    ShowWindow(hWnd, iShowCmd);

    // Paint the background of window
    UpdateWindow(hWnd);

    // Initialize
    int result = initialize();
    if (result != 0)
    {
        fprintf(gpFile, "ERROR : initialize() function failed...!\n");
        DestroyWindow(hWnd);
        hWnd = NULL;
    }
    else
    {
        fprintf(gpFile, "initialize() function completed successfully...!\n");
    }

    // Set this window as foreground and active window
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
    void uninitialize();
    void toggleFullScreen(void);

    // Code

    switch (uMsg)
    {
    case WM_CREATE:
        // Begin Playing The Music
        PlaySound(MAKEINTRESOURCE(MYSONG), GetModuleHandle(NULL), SND_ASYNC | SND_LOOP | SND_RESOURCE);

        SetTimer(hWnd, MYTIMER, 119000, NULL);

        ZeroMemory((void *)&wpPrev, sizeof(WINDOWPLACEMENT));
        wpPrev.length = sizeof(WINDOWPLACEMENT);
        break;

    case WM_SETFOCUS:
        gbActiveWindow = TRUE;
        break;

    case WM_KILLFOCUS:
        gbActiveWindow = FALSE;
        break;

    case WM_ERASEBKGND:
        return (0);

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

    case WM_TIMER:
        KillTimer(hWnd, MYTIMER);
        DestroyWindow(hWnd);
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

    // Default window procedure
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void toggleFullScreen(void)
{
    // Variable declaration's
    MONITORINFO mi;

    if (gbFullScreen == FALSE)
    {
        dwStyle = GetWindowLong(ghWnd, GWL_STYLE);
        if (dwStyle & WS_OVERLAPPEDWINDOW)
        {
            ZeroMemory((void *)&mi, sizeof(MONITORINFO));
            mi.cbSize = sizeof(MONITORINFO);

            if (GetWindowPlacement(ghWnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghWnd, MONITORINFOF_PRIMARY), &mi))
            {
                SetWindowLong(ghWnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
                SetWindowPos(ghWnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.left, SWP_NOZORDER | SWP_FRAMECHANGED);
            }
        }
        ShowCursor(FALSE);
    }
    else
    {
        SetWindowPlacement(ghWnd, &wpPrev);
        SetWindowLong(ghWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPos(ghWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
        ShowCursor(TRUE);
    }
}

int initialize(void)
{
    // Local function declaration's
    void printGLInfo(void);
    void resize(int, int);

    // Variable declaration's
    PIXELFORMATDESCRIPTOR pfd;
    int iPixelFormatIndex = 0;

    // Code

    // PixelFormatDescriptor initialization
    ZeroMemory((void *)&pfd, sizeof(PIXELFORMATDESCRIPTOR));

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cRedBits = 8;
    pfd.cGreenBits = 8;
    pfd.cBlueBits = 8;
    pfd.cAlphaBits = 8;

    // GetDC
    ghdc = GetDC(ghWnd);
    if (ghdc == NULL)
    {
        fprintf(gpFile, "ERROR : GetDC() function failed...!\n");
        return (-1);
    }

    // Get matching pixel format index using hdc and pfd
    iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
    if (iPixelFormatIndex == 0)
    {
        fprintf(gpFile, "ERROR : ChoosePixelFormat() function failed...!\n");
        return (-2);
    }

    // Set the pixel format of found index
    if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
    {
        fprintf(gpFile, "ERROR : SetPixelFormat() function failed...!\n");
        return (-3);
    }

    // Create rendering context using hdc, pfd and choosen pixelformatindex
    ghrc = wglCreateContext(ghdc);
    if (ghrc == NULL)
    {
        fprintf(gpFile, "ERROR : wglCreateContext() function failed...!\n");
        return (-4);
    }

    // Make this rendering context as current context
    if (wglMakeCurrent(ghdc, ghrc) == FALSE)
    {
        fprintf(gpFile, "ERROR : wglMakeCurrent() function failed...!\n");
        return (-5);
    }

    // Print OpenGL info
    printGLInfo();

    // From here onwords OpenGL code start

    // Tell OpenGL to choose the color to clear the screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Warm-up resize
    resize(WIN_WIDTH, WIN_HEIGHT);

    return (0);
}

void printGLInfo(void)
{
    // Code

    // Print OpenGL information

    fprintf(gpFile, "--- OPENGL INFORMATION ---\n");
    fprintf(gpFile, "************************************************\n\n");

    fprintf(gpFile, "OpenGL Vendor : %s\n", glGetString(GL_VENDOR));
    fprintf(gpFile, "OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
    fprintf(gpFile, "OpenGL Version : %s\n\n", glGetString(GL_VERSION));

    fprintf(gpFile, "************************************************\n");
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
    void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3);
    void drawQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
    void DrawBharat(void);
    void DrawPlane1(void);
    void DrawPlane2(void);
    void DrawPlane3(void);

    // Code

    // Clear OpenGL buffers
    glClear(GL_COLOR_BUFFER_BIT);

    // Set matrix to modelview mode
    glMatrixMode(GL_MODELVIEW);

    // Set to identity matrix
    glLoadIdentity();

    // Translate triangle backwords by z
    glTranslatef(0.0f, 0.0f, -4.0f);

    DrawBharat();

    DrawPlane1();

    DrawPlane2();

    DrawPlane3();

    // Swap the buffers
    SwapBuffers(ghdc);
}

void update(void)
{
    // Code

    // X1 = -2.0f + 2.75f * sin(angle);
    // Y1 = -2.75f + 2.75f * cos(angle);

    if (bDoneLetters)
    {
        if (X1 < 1.95f)
        {
            X1 = X1 + 0.00005f;
            Y1 = Y1 + 0.00006f;
            if (Y1 > -0.03f)
            {
                Y1 = -0.03f;
            }
        }
        else
        {
            X1 = X1 + 0.00005f;
            Y1 = Y1 - 0.00006f;
        }

        if (X2 < 1.95f)
        {
            X2 = X2 + 0.00005f;
            Y2 = Y2 - 0.00006f;
            if (Y2 < 0.03f)
            {
                Y2 = 0.03f;
            }
        }
        else
        {
            X2 = X2 + 0.00005f;
            Y2 = Y2 + 0.00006f;
        }

        X3 = X3 + 0.00005f;
        
        if (X3 >= -1.55f)
        {
            Bpass = TRUE;
        }

        if (X3 >= -0.8)
        {
            Hpass = TRUE;
        }

        if (X3 >= -0.15)
        {
            A1pass = TRUE;
        }

        if (X3 >= 0.5)
        {
            Rpass = TRUE;
        }

        if (X3 >= 1.2)
        {
            A2pass = TRUE;
        }

        if (X3 >= 1.65)
        {
            Tpass = TRUE;
        }

        
        if(X3 >= -0.85)
        {
            bH = TRUE;
        }
    }

    if (iLetterNumber == 1)
    {
        XB = XB + 0.00006;
        if (XB > 0.0f)
        {
            XB = 0.0f;
            iLetterNumber = 2;
        }
    }
    else if(iLetterNumber == 2)
    {
        XH = XH + 0.00006f;
        if(XH >= 0.0f)
        {
            XH = 0.0f;
        }

        YH = YH - 0.00006f;
        if(YH < 0.0f)
        {
            YH = 0.0f;
        }

        if(XH == 0.0f && YH == 0.0f)
        {
            iLetterNumber = 3;
        }
    }
    else if(iLetterNumber == 3)
    {
        XA1 = XA1 + 0.00006f;
        if(XA1 >= 0.0f)
        {
            XA1 = 0.0f;
        }

        YA1 = YA1 + 0.00006f;
        if(YA1 >= 0.0f)
        {
            YA1 = 0.0f;
        }

        if(XA1 == 0.0f && YA1 == 0.0f)
        {
            iLetterNumber = 4;
        }
    }
    else if(iLetterNumber == 4)
    {
        XR = XR - 0.00006f;
        if(XR <= 0.0f)
        {
            XR = 0.0f;
        }

        YR = YR - 0.00006f;
        if(YR <= 0.0f)
        {
            YR = 0.0f;
        }

        if(XR == 0.0f && YR == 0.0f)
        {
            iLetterNumber = 5;
        }
    }
    else if(iLetterNumber == 5)
    {
        XA2 = XA2 - 0.00006f;
        if(XA2 <= 0.0f)
        {
            XA2 = 0.0f;
        }

        YA2 = YA2 + 0.00006f;
        if(YA2 >= 0.0f)
        {
            YA2 = 0.0f;
        }

        if(XA2 == 0.0f && YA2 == 0.0f)
        {
            iLetterNumber = 6;
        }
    }
    else if (iLetterNumber == 6)
    {
        XT = XT - 0.00006f;
        if (XT < 0.0f)
        {
            iLetterNumber = 7;
            bDoneLetters = TRUE;
        }
    }
    

    if(tail)
    {
        t = -0.35f;
        tail = FALSE;
    }
    else
    {
        t = -0.45f;
        tail = TRUE;
    }
}

void DrawPlane1(void)
{
    glPushMatrix();

    glTranslatef(X1, Y1, 0.0f);

    // glRotatef(angle1, 0.0, 0.0, 1.0f);

    drawTriangle(0.0f, 0.0f, -0.02f, 0.02f, -0.02f, -0.02);

    glColor3f(0.5f, 0.5f, 0.5f);
    drawQuad(-0.02f, 0.02f, -0.2f, 0.02f, -0.2f, -0.02f, -0.02f, -0.02f);

    drawQuad(-0.03f, 0.00f, -0.12f, 0.08f, -0.12f, -0.08f, -0.03f, 0.00f);

    glColor3f(0.0f, 1.0f, 0.0f);
    drawTriangle(-0.2f, 0.02f, t, 0.01f, -0.2f, 0.0f);
    drawTriangle(-0.2f, 0.0f, t, -0.01f, -0.2f, -0.02f);

    glColor3f(0.0f, 1.0f, 0.0f);
    drawQuad(-0.02f, 0.01f, -0.15f, 0.01f, -0.15f, -0.01, -0.02f, -0.01f);

    glPopMatrix();
}

void DrawPlane2(void)
{
    glPushMatrix();

    glTranslatef(X2, Y2, 0.0f);

    // glRotatef(angle1, 0.0, 0.0, 1.0f);

    drawTriangle(0.0f, 0.0f, -0.02f, 0.02f, -0.02f, -0.02);

    glColor3f(0.5f, 0.5f, 0.5f);
    drawQuad(-0.02f, 0.02f, -0.2f, 0.02f, -0.2f, -0.02f, -0.02f, -0.02f);

    drawQuad(-0.03f, 0.00f, -0.12f, 0.08f, -0.12f, -0.08f, -0.03f, 0.00f);

    glColor3f(1.0f, 0.6f, 0.2f);
    drawTriangle(-0.2f, 0.02f, t, 0.01f, -0.2f, 0.0f);
    drawTriangle(-0.2f, 0.0f, t, -0.01f, -0.2f, -0.02f);

    glColor3f(1.0f, 0.6f, 0.2f);
    drawQuad(-0.02f, 0.01f, -0.15f, 0.01f, -0.15f, -0.01, -0.02f, -0.01f);

    glPopMatrix();
}

void DrawPlane3(void)
{
    glPushMatrix();

    glTranslatef(X3, Y3, 0.0f);

    // glRotatef(angle1, 0.0, 0.0, 1.0f);

    drawTriangle(0.0f, 0.0f, -0.02f, 0.02f, -0.02f, -0.02);

    glColor3f(0.5f, 0.5f, 0.5f);
    drawQuad(-0.02f, 0.02f, -0.2f, 0.02f, -0.2f, -0.02f, -0.02f, -0.02f);

    drawQuad(-0.03f, 0.00f, -0.12f, 0.08f, -0.12f, -0.08f, -0.03f, 0.00f);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawTriangle(-0.2f, 0.02f, t, 0.01f, -0.2f, 0.0f);
    drawTriangle(-0.2f, 0.0f, t, -0.01f, -0.2f, -0.02f);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawQuad(-0.02f, 0.01f, -0.15f, 0.01f, -0.15f, -0.01, -0.02f, -0.01f);

    glPopMatrix();
}

void DrawBharat(void)
{
    // Local function declaration's
    void drawQuad_1(float, float, float, float);
    void drawQuad_2(float x1, float y1, float x2, float y2, float tempFact);
    void drawQuadAnother(float x1, float x2, float x3, float x4, float y1, float y2, float y3, float y4);
    void drawLine(float x1, float y1, float x2, float y2, float r, float g, float b);

    glColor3f(0.5f, 0.5f, 0.5f);

    glPushMatrix();

    glTranslatef(XB, 0.0f, 0.0f);

    // Draw 'B'
    drawQuad_1(-1.75f, 0.4f, -1.85f, -0.4f);

    // Horizontal lines
    drawQuadAnother(-1.55f, -1.9f, -1.9f, -1.55f, 0.4f, 0.4f, 0.3f, 0.3f);
    drawQuadAnother(-1.55f, -1.9f, -1.9f, -1.55f, -0.3f, -0.3f, -0.4f, -0.4f);

    // Vertical lines
    drawQuadAnother(-1.45f, -1.55f, -1.55f, -1.44f, 0.35f, 0.4f, 0.0f, 0.05f);
    drawQuadAnother(-1.45f, -1.55f, -1.55f, -1.44f, -0.05f, 0.0f, -0.4f, -0.35f);

    //drawQuadAnother(-1.55f, -1.9f, -1.9f, -1.55f, 0.05f, 0.05f, -0.05f, -0.05);

    if (Bpass)
    {
        drawLine(-1.9f, 0.03f, -1.55f, 0.03f, 1.0f, 0.4039f, 0.1215f);
        drawLine(-1.9f, 0.0f, -1.55f, 0.0f, 1.0f, 1.0f, 1.0f);
        drawLine(-1.9f, -0.03f, -1.55f, -0.03f, 0.0156f, 0.4156f, 0.2196f);
    }
    else
    {
        drawLine(-1.9f, 0.03f, -1.55f, 0.03f, 0.5f, 0.5f, 0.5f);
        drawLine(-1.9f, 0.0f, -1.55f, 0.0f, 0.5f, 0.5f, 0.5f);
        drawLine(-1.9f, -0.03f, -1.55f, -0.03f, 0.5f, 0.5f, 0.5f);
    }

    glPopMatrix();

    glPushMatrix();

    glTranslatef(XH, YH, 0.0f);

    // Draw 'H'
    if (Hpass)
    {
        drawLine(-1.2f, 0.03f, -0.8, 0.03f, 1.0f, 0.4039f, 0.1215f);
        drawLine(-1.2f, 0.0f, -0.8, 0.0f, 1.0f, 1.0f, 1.0f);
        drawLine(-1.2f, -0.03f, -0.8, -0.03f, 0.0156f, 0.4156f, 0.2196f);
    }
    else
    {
        drawLine(-1.2f, 0.03f, -0.8, 0.03f, 0.5f, 0.5f, 0.5f);
        drawLine(-1.2f, 0.0f, -0.8, 0.0f, 0.5f, 0.5f, 0.5f);
        drawLine(-1.2f, -0.03f, -0.8, -0.03f, 0.5f, 0.5f, 0.5f);
    }

    glColor3f(0.5f, 0.5f, 0.5f);
	drawQuad_1(-1.1f, 0.4f, -1.2f, -0.4f);
	drawQuad_1(-0.75f, 0.4f, -0.85f, -0.4f);

    glPopMatrix();

    glPushMatrix();

    glTranslatef(XA1, YA1, 0.0f);

    // Draw 'A'

    if (A1pass)
    {
        drawLine(-0.4f, 0.03f, -0.15f, 0.03f, 1.0f, 0.4039f, 0.1215f);
        drawLine(-0.4f, 0.0f, -0.15f, 0.0f, 1.0f, 1.0f, 1.0f);
        drawLine(-0.4f, -0.03f, -0.15f, -0.03f, 0.0156f, 0.4156f, 0.2196f);
    }
    else
    {
        drawLine(-0.4f, 0.03f, -0.15f, 0.03f, 0.5f, 0.5f, 0.5f);
        drawLine(-0.4f, 0.0f, -0.15f, 0.0f, 0.5f, 0.5f, 0.5f);
        drawLine(-0.4f, -0.03f, -0.15f, -0.03f, 0.5f, 0.5f, 0.5f);
    }

    glColor3f(0.5f, 0.5f, 0.5f);
	drawQuad_2(-0.25f, 0.4f, -0.35f, -0.4f, 0.1f);
	drawQuad_2(-0.3f, 0.4f, -0.20f, -0.4f, -0.1);

    glPopMatrix();


    glPushMatrix();

    glTranslatef(XR, YR, 0.0f);

    // Draw 'R'
    glColor3f(0.5f, 0.5f, 0.5f);
	drawQuad_1(0.3f, 0.4f, 0.2f, -0.4f);
	drawQuad_2(0.2f, 0.0f, 0.31f, -0.4f, 0.0);

	// Horizontal lines
	drawQuad(0.5f, 0.4f, 0.15f, 0.4f, 0.15f, 0.3f, 0.5f, 0.3f);
    drawQuad(0.6f, 0.35f, 0.5f, 0.4f, 0.5f, 0.0f, 0.6f, 0.05f);

    if (Rpass)
    {
        drawLine(0.15f, 0.03f, 0.5f, 0.03f, 11.0f, 0.4039f, 0.1215f);
        drawLine(0.15f, 0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f);
        drawLine(0.15f, -0.03f, 0.5f, -0.03f, 0.0156f, 0.4156f, 0.2196f);
    }
    else
    {
        drawLine(0.15f, 0.03f, 0.5f, 0.03f, 0.5f, 0.5f, 0.5f);
        drawLine(0.15f, 0.0f, 0.5f, 0.0f, 0.5f, 0.5f, 0.5f);
        drawLine(0.15f, -0.03f, 0.5f, -0.03f, 0.5f, 0.5f, 0.5f);
    }
	

    glPopMatrix();

    glPushMatrix();

    glTranslatef(XA2, YA2, 0.0f);
    
    // Draw 'A'
    if (A2pass)
    {
        drawLine(0.95f, 0.03f, 1.2f, 0.03f, 1.0f, 0.4039f, 0.1215f);
        drawLine(0.95f, 0.0f, 1.2f, 0.0f, 1.0f, 1.0f, 1.0f);
        drawLine(0.95f, -0.03f, 1.2f, -0.03f, 0.0156f, 0.4156f, 0.2196f);
    }
    else
    {
        drawLine(0.95f, 0.03f, 1.2f, 0.03f, 0.5f, 0.5f, 0.5f);
        drawLine(0.95f, 0.0f, 1.2f, 0.0f, 0.5f, 0.5f, 0.5f);
        drawLine(0.95f, -0.03f, 1.2f, -0.03f, 0.5f, 0.5f, 0.5f);
    }

    glColor3f(0.5f, 0.5f, 0.5f);
    glColor3f(0.5f, 0.5f, 0.5f);
	drawQuad_2(1.1f, 0.4f, 1.0f, -0.4f, 0.1f);
	drawQuad_2(1.05f, 0.4f, 1.15f, -0.4f, -0.1);

    glPopMatrix();

    glPushMatrix();

    glTranslatef(XT, 0.0f, 0.0f);

    // Draw 'T'
    glColor3f(0.5f, 0.5f, 0.5f);
    drawQuad(1.8, 0.4f, 1.4f, 0.4f, 1.4f, 0.3f, 1.8, 0.3f);

    drawQuad_1(1.65f, 0.4f, 1.55f, -0.4f);

    if (Tpass)
    {
        drawLine(1.55f, 0.03f, 1.65f, 0.03f, 1.0f, 0.4039f, 0.1215f);
        drawLine(1.55f, 0.0f, 1.65f, 0.0f, 1.0f, 1.0f, 1.0f);
        drawLine(1.55f, -0.03f, 1.65f, -0.03f, 0.0156f, 0.4156f, 0.2196f);
    }
    else
    {
        drawLine(1.55f, 0.03f, 1.65f, 0.03f, 0.5f, 0.5f, 0.5f);
        drawLine(1.55f, 0.0f, 1.65f, 0.0f, 0.5f, 0.5f, 0.5f);
        drawLine(1.55f, -0.03f, 1.65f, -0.03f, 0.5f, 0.5f, 0.5f);
    }

    glPopMatrix();
}

void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3)
{
    glBegin(GL_TRIANGLES);

    glVertex3f(x1, y1, 0.0f);
    glVertex3f(x2, y2, 0.0f);
    glVertex3f(x3, y3, 0.0f);

    glEnd();
}

void drawQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
    glBegin(GL_QUADS);

    glVertex3f(x1, y1, 0.0f);
    glVertex3f(x2, y2, 0.0f);
    glVertex3f(x3, y3, 0.0f);
    glVertex3f(x4, y4, 0.0f);

    glEnd();
}

void drawQuadAnother(float x1, float x2, float x3, float x4, float y1, float y2, float y3, float y4)
{
    glBegin(GL_QUADS);

    glVertex3f(x1, y1, 0.0f);

    glVertex3f(x2, y2, 0.0f);

    glVertex3f(x3, y3, 0.0f);

    glVertex3f(x4, y4, 0.0f);

    glEnd();
}

void drawQuad_1(float x1, float y1, float x2, float y2)
{
    glBegin(GL_QUADS);

    glVertex3f(x1, y1, 0.0f);
    glVertex3f(x2, y1, 0.0f);

    glVertex3f(x2, 0.0f, 0.0f);
    glVertex3f(x1, 0.0f, 0.0f);

    glVertex3f(x1, 0.0f, 0.0f);
    glVertex3f(x2, 0.0f, 0.0f);

    glVertex3f(x2, y2, 0.0f);
    glVertex3f(x1, y2, 0.0f);

    glEnd();
}

void drawQuad_2(float x1, float y1, float x2, float y2, float tempFact)
{
	glBegin(GL_QUADS);

	glVertex3f(x1, y1, 0.0f);
	glVertex3f(x2, y1, 0.0f);

	glVertex3f(x2 - tempFact, 0.0f, 0.0f);
	glVertex3f(x1 - tempFact, 0.0f, 0.0f);

	glVertex3f(x1 - tempFact, 0.0f, 0.0f);
	glVertex3f(x2 - tempFact, 0.0f, 0.0f);

	if (tempFact > 0)
	{
		glVertex3f(x2 - (tempFact + 0.1f), y2, 0.0f);
		glVertex3f(x1 - (tempFact + 0.1f), y2, 0.0f);
	}
	else if(tempFact < 0)
	{
		glVertex3f(x2 + (tempFact + 0.3f), y2, 0.0f);
		glVertex3f(x1 + (tempFact + 0.3f), y2, 0.0f);
	}
	else
	{
		glVertex3f(x2 + (tempFact + 0.3f), y2, 0.0f);
		glVertex3f(x1 + (tempFact + 0.3f), y2, 0.0f);
	}
	glEnd();
}

void drawLine(float x1, float y1, float x2, float y2, float r, float g, float b)
{
    glLineWidth(9.0f);
    glBegin(GL_LINES);

    glColor3f(r, g, b);
    glVertex3f(x1, y1, 0.0f);
    glVertex3f(x2, y2, 0.0f);

    glEnd();
}

void uninitialize(void)
{
    // Local function declaration's
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
        fprintf(gpFile, "\n");
        fprintf(gpFile, "Program terminated successfully...!\n\n");
        fclose(gpFile);
        gpFile = NULL;
    }
}
