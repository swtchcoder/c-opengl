#ifndef _WINDOW_H
#define _WINDOW_H

/*
https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
*/
typedef enum {
	INPUT_LBUTTON = 0x01,
	INPUT_RBUTTON,
	INPUT_MBUTTON,
	INPUT_BACK = 0x08,
	INPUT_TAB,
	INPUT_RETURN = 0x0d,
	INPUT_SHIFT = 0x10,
	INPUT_CONTROL,
	INPUT_MENU,
	INPUT_PAUSE,
	INPUT_CAPITAL,
	INPUT_ESCAPE = 0x1b,
	INPUT_SPACE = 0x20,
	INPUT_LEFT = 0x25,
	INPUT_RIGHT,
	INPUT_UP,
	INPUT_DOWN,
	INPUT_0 = 0x30,
	INPUT_1,
	INPUT_2,
	INPUT_3,
	INPUT_4,
	INPUT_5,
	INPUT_6,
	INPUT_7,
	INPUT_8,
	INPUT_9,
	INPUT_A = 0x41,
	INPUT_B,
	INPUT_C,
	INPUT_D,
	INPUT_E,
	INPUT_F,
	INPUT_G,
	INPUT_H,
	INPUT_I,
	INPUT_J,
	INPUT_K,
	INPUT_L,
	INPUT_M,
	INPUT_N,
	INPUT_O,
	INPUT_P,
	INPUT_Q,
	INPUT_R,
	INPUT_S,
	INPUT_T,
	INPUT_U,
	INPUT_V,
	INPUT_W,
	INPUT_X,
	INPUT_Y,
	INPUT_Z,
	INPUT_F1 = 0x70,
	INPUT_F2,
	INPUT_F3,
	INPUT_F4,
	INPUT_F5,
	INPUT_F6,
	INPUT_F7,
	INPUT_F8,
	INPUT_F9,
	INPUT_F10,
	INPUT_F11,
	INPUT_F12
} input_t;

void window_create(const char *name, const int resizable, const int width, const int height);
int window_update(void);
int input_mouse_x(void);
int input_mouse_y(void);
int input_key_down(input_t key);
void window_destroy(void);

#endif