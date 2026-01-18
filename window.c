#include "window.h"
#include <stdlib.h>
#include "glad/glad.h"
#include <windows.h>
#include <windowsx.h>

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
static int mouse_x, mouse_y;
static const char *vertex =
	"#version 330 core\n"
	"layout (location = 0) in vec2 pos;\n"
	"void main() {\n"
	"    gl_Position = vec4(pos, 0.0, 1.0);\n"
	"}\n";
static const char *fragment =
	"#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main() {\n"
	"    FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
	"}\n";
static GLuint program;
static GLuint vao, vbo;

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
	if (!gladLoadGL()) {
		MessageBox(NULL, TEXT("Could not load GLAD"), NULL, MB_ICONERROR);
		exit(EXIT_FAILURE);
	}
	/* setup shader program */
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment, NULL);
	glCompileShader(fs);
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glDeleteShader(vs);
	glDeleteShader(fs);
	/* setup triangle buffers */
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

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

int
input_mouse_x(void)
{
	return mouse_x;
}

int
input_mouse_y(void)
{
	return mouse_y;
}

int
input_key_down(input_t key) {
	return keys[key];
}

void
window_triangle(float x_1, float y_1, float x_2, float y_2, float x_3, float y_3)
{
	float vertices[6] = {
		x_1, y_1,
		x_2, y_2,
		x_3, y_3
	};
	glUseProgram(program);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}

void
window_destroy(void)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(ctx);
	ReleaseDC(hwnd, hdc);
	DestroyWindow(hwnd);

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(program);

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
	case WM_MOUSEMOVE:
		mouse_x = GET_X_LPARAM(lParam);
		mouse_y = GET_Y_LPARAM(lParam);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}