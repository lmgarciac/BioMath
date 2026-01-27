#define WIN32_LEAN_AND_MEAN //Minimal basic WinApi
#include <windows.h>
#include <gl/GL.h>

#pragma comment(lib, "opengl32.lib")

static const wchar_t* kClassName = L"BioMathWindow";

// Handlers (encargados de procesar eventos):

// HWND: Handle to Window
// HDC: Handle to device context
// HGLRC: Handle to OpenGL rendering context

static HDC g_hdc = nullptr;
static HGLRC g_glrc = nullptr;
static bool g_running = true;
	
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;
	case WM_DESTROY:
		g_running = false;
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

static bool InitWGL(HWND hwnd)
{
	g_hdc = GetDC(hwnd);
	if (!g_hdc) return false;

	PIXELFORMATDESCRIPTOR pfd{}; // Como sera el framebuffer?
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24; // Z-Buffer bits (mientras más, mas precisa la comparacion (menos Z fighting).

	// Una linda definición del stencil buffer:
	// Un buffer auxiliar, por píxel, que participa en pruebas (tests) durante el pipeline 
	// de rasterización para decidir si un fragmento se procesa o se descarta.
	pfd.cStencilBits = 8;

	pfd.iLayerType = PFD_MAIN_PLANE;

	int pf = ChoosePixelFormat(g_hdc, &pfd);
	if (pf == 0) return false;

	if (!SetPixelFormat(g_hdc, pf, &pfd)) return false;

	g_glrc = wglCreateContext(g_hdc);
	if (!g_glrc) return false;

	// Asocio el contexto OpenGL (HGLRC) con el Device Context (HDC) en el thread actual, haciendo que OpenGL quede activo.
	if (!wglMakeCurrent(g_hdc,g_glrc)) return false;
}

static void ShutdownWGL(HWND hwnd)
{
	if (g_glrc)
	{
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(g_glrc);
		g_glrc = nullptr;
	}

	if (g_hdc)
	{
		ReleaseDC(hwnd, g_hdc);
		g_hdc = nullptr;
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	WNDCLASS wc{};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = kClassName;

	if (!RegisterClass(&wc))
		return 1;

	HWND hWnd = CreateWindowEx(
		0, kClassName, L"WIP BioMath",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
		nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
		return 1;

	ShowWindow(hWnd, SW_SHOW);

	if (!InitWGL(hWnd))
		return 1;

	MSG msg{};
	while (g_running)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				g_running = false;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Clear to solid color
		glViewport(0, 0, 1280, 720); // Por ahora fijo
		glClearColor(0.08f, 0.10f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		SwapBuffers(g_hdc);

		Sleep(1);
	}

	ShutdownWGL(hWnd);
	return 0;
}