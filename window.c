#include "window.h"
#include <stdlib.h>
#include <windows.h>
#include <GL/gl.h>

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static const char class_name[] = "MainWindowClass";
static HWND hwnd;
static HDC hdc;
static HGLRC ctx;
static MSG msg;
static const PIXELFORMATDESCRIPTOR pfd = {
	sizeof(PIXELFORMATDESCRIPTOR),
	1,
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
	PFD_TYPE_RGBA,
	32,
	0, 0,
	0,0,
	0,0, 
	0, 0,
	0,
	0, 0, 0, 0,
	24,
	8,
	0,
	PFD_MAIN_PLANE, 
	0, 
	0, 0, 0
};
static int keys[0xff];

void
window_create(const char *name, const int resizable, const int width, const int height)
{
	WNDCLASS wc = {0};
	int pixel_format;
	DWORD style;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = class_name;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	if (!RegisterClass(&wc)) {
		MessageBox(NULL, TEXT("Could not register window class"), NULL, MB_ICONERROR);
		exit(EXIT_FAILURE);
	}
	if (resizable) {
		style = WS_OVERLAPPEDWINDOW;
	} else {
		style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	}
	hwnd = CreateWindowA(
		class_name, 
		name,
		style,
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		width, 
		height, 
		NULL, 
		NULL,
		wc.hInstance, 
		NULL
	);
	if (!hwnd) {
		MessageBox(NULL, TEXT("Could not create window"), NULL, MB_ICONERROR);
		exit(EXIT_FAILURE);
	}
	hdc = GetDC(hwnd);
	pixel_format = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pixel_format, &pfd);
	ctx = wglCreateContext(hdc);
	if (!ctx) {
		MessageBox(NULL, TEXT("Could not create OpenGL context"), NULL, MB_ICONERROR);
		exit(EXIT_FAILURE);
	}
	if (!wglMakeCurrent(hdc, ctx)) {
		MessageBox(NULL, TEXT("Could not make OpenGL context current"), NULL, MB_ICONERROR);
		exit(EXIT_FAILURE);
	}
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
}

int
window_update(void)
{
	if (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) {
			return 0;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	SwapBuffers(hdc);
	return 1;
}

int input_key_down(input_t key) {
	return keys[key];
}

void
window_destroy(void)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(ctx);
	ReleaseDC(hwnd, hdc);
	DestroyWindow(hwnd);
}

static LRESULT CALLBACK
WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		keys[wParam] = 1;
		return 0;
	case WM_KEYUP:
		keys[wParam] = 0;
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}