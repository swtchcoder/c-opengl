#include "window.h"
#include <GL/gl.h>

int step(void);
void draw(void);

static const char *window_name = "Hello world!";
static const int width = 800;
static const int height = 600;

int
main()
{
	window_create(window_name, 1, width, height);
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	while (step()) {
		draw();
	}
	window_destroy();
	return 0;
}

int
step()
{
	if (!window_update()) {
		return 0;
	}
	if (input_key_down(INPUT_ESCAPE)) {
		return 0;
	}
	return 1;
}

void
draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
}