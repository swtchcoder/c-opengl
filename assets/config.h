typedef struct {
	const char *filename;
	const char *name;
} source_t;

typedef struct {
	const char *filename;
	const char *name;
	const float size;
} font_t;

const source_t textuals[] = {
	{"main.c", "main"},
	{"config.h", "config"}
};

const source_t images[] = {
	{"image.jpg", "image"}
};

const font_t fonts[] = {
	{"OpenSans.ttf", "open_sans", 16.0f}
};