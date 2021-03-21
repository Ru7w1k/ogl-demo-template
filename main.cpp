// headers
#include "main.h"
#include "helper.h"
#include "logger.h"

// scenes
#include "scene.h"
#include "scene-triangle.h"
#include "scene-square.h"

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

// macros
#define WIN_WIDTH  800
#define WIN_HEIGHT 600

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// global variables
HDC   ghdc = NULL;
HGLRC ghrc = NULL;

bool gbFullscreen = false;
bool gbActiveWindow = false;

HWND  ghwnd = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

int gWidth  = 0;
int gHeight = 0;

bool bPause = false;

// WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// function declarations
	void initialize(void);
	void display(void);
	void update(float);

	// variable declarations
	bool bDone = false;
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyApp");
	float delta;

	// code

	// initialization of WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(RMC_ICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(RMC_ICON));

	// register above class
	RegisterClassEx(&wndclass);

	// get the screen size
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);

	// create window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("OpenGL | Sandbox"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		(width / 2) - (WIN_WIDTH / 2),
		(height / 2) - (WIN_HEIGHT / 2),
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	initialize();

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Game Loop!
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == true)
			{
				delta = GetTimeDeltaMS();
				// call update() here for OpenGL rendering
				if (!bPause) update(delta);
				// call display() here for OpenGL rendering
				display();
			}
		}
	}

	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// function declaration
	void display(void);
	void resize(int, int);
	void uninitialize();
	void ToggleFullscreen(void);

	// code
	switch (iMsg)
	{

	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = false;
		break;

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;

		case 0x46:
		case 0x66:
			ToggleFullscreen();
			break;

		case VK_LEFT:
			PrevScene();
			break;

		case VK_RIGHT:
			NextScene();
			break;

		case VK_SPACE:
			bPause = !bPause;
			break;

		default:
			break;
		}
		break;

	case WM_ERASEBKGND:
		return(0);

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		uninitialize();
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
	// local variables
	MONITORINFO mi = { sizeof(MONITORINFO) };

	// code
	if (gbFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) &&
				GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP,
					mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		gbFullscreen = true;
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP,
			0, 0, 0, 0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
		gbFullscreen = false;
	}
}


void initialize(void)
{
	// function declarations
	void resize(int, int);
	void uninitialize(void);

	// variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	// code
	ghdc = GetDC(ghwnd);

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

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		LogD("ChoosePixelFormat() failed..");
		DestroyWindow(ghwnd);
		return;
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		LogD("SetPixelFormat() failed..");
		DestroyWindow(ghwnd);
		return;
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		LogD("wglCreateContext() failed..");
		DestroyWindow(ghwnd);
		return;
	}

	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		LogD("wglMakeCurrent() failed..");
		DestroyWindow(ghwnd);
		return;
	}

	// glew initialization for programmable pipeline
	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		LogD("glewInit() failed..");
		DestroyWindow(ghwnd);
		return;
	}

	// init logger
	InitLogger();

	// fetch OpenGL related details
	LogD("OpenGL Vendor:   %s", glGetString(GL_VENDOR));
	LogD("OpenGL Renderer: %s", glGetString(GL_RENDERER));
	LogD("OpenGL Version:  %s", glGetString(GL_VERSION));
	LogD("GLSL Version:    %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// fetch OpenGL enabled extensions
	GLint numExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	LogD("==== OpenGL Extensions ====");
	for (int i = 0; i < numExtensions; i++)
	{
		LogD("  %s", glGetStringi(GL_EXTENSIONS, i));
	}
	LogD("===========================\n");	

	// initialize scene queue
	InitSceneQueue();

	// add scenes
	AddScene(GetTriangleScene());
	AddScene(GetSquareScene());

	// initialize all scenes
	for (int i = 0; i < GetSceneCount(); i++)
	{
		Scene scene;
		if (GetSceneAt(scene, i))
		{
			if (!scene.InitFunc())
			{
				LogE("Scene %s Init() failed..", scene.Name);
				DestroyWindow(ghwnd);
				return;
			}
			LogD("Scene %s Init() done..", scene.Name);

			// warm-up resize
			scene.ResizeFunc(WIN_WIDTH, WIN_HEIGHT);
			LogD("Scene %s Resize() done..", scene.Name);
		}
	}

	// set clear color and clear depth
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	// depth test 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	// clock for syncing animation speed
	InitClock();

	// warm-up resize call
	resize(WIN_WIDTH, WIN_HEIGHT);
}

void resize(int width, int height)
{
	// code
	if (height == 0)
		height = 1;

	gWidth  = width;
	gHeight = height;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	// resize active scene
	Scene scene;
	if (GetScene(scene)) scene.ResizeFunc(width, height);
}

void display(void)
{
	// code

	// draw active scene
	Scene scene;
	if (GetScene(scene)) scene.DisplayFunc();

	SwapBuffers(ghdc);
}

void update(float delta)
{
	// code	
	
	// update active scene
	Scene scene;
	if (GetScene(scene))
	{
		if (scene.UpdateFunc(delta))
		{
			// current scene is completed
			// move to the next scene
			LogD("Scene %s Update() done..", scene.Name);
			RemoveScene();

			// if no next scene, terminate
			if (!GetScene(scene))
			{
				DestroyWindow(ghwnd);
				return;
			}

			// warm-up resize call to next scene
			scene.ResizeFunc(gWidth, gHeight);
			LogD("Scene %s Resize() done..", scene.Name);
		}
	}
}

void uninitialize(void)
{
	// code
	if (gbFullscreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);

		ShowCursor(TRUE);
	}

	// uninit all scenes
	for (int i = 0; i < GetSceneCount(); i++)
	{
		Scene scene;
		if (GetSceneAt(scene, i))
		{
			LogD("Scene %s Uninit() calling..", scene.Name);
			scene.UninitFunc();
		}
	}

	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	UninitLogger();
}


