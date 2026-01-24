#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"
#include "config.h"

static void textual_process(void);
static void image_process(void);
static void font_process(void);
static size_t file_load(const char *path, unsigned char **buffer);
static size_t file_read(FILE *f, unsigned char **buffer);
static size_t file_size(FILE *f);
static int pow2_next(int a);

static FILE *data;
static FILE *meta;
static const size_t textual_count = sizeof(textuals) / sizeof(source_t);
static source_t source;
static const font_t *source_font;
static FILE *f;
static const size_t image_count = sizeof(images) / sizeof(source_t);
static const size_t font_count = sizeof(fonts) / sizeof(font_t);
static size_t size;
static size_t index = 0;

int
main(void)
{
	size_t i;
	data = fopen("assets.bin", "wb");
	if (!data) {
		perror("assets.bin");
		return 1;
	}
	meta = fopen("assets_meta.h", "w");
	if (!meta) {
		perror("assets_meta.h");
		return 1;
	}
	for (i = 0; i < textual_count; i++) {
		source = textuals[i];
		textual_process();
	}
	for (i = 0; i < image_count; i++) {
		source = images[i];
		image_process();
	}
	for (i = 0; i < font_count; i++) {
		source_font = &fonts[i];
		font_process();
	}
	fclose(data);
	fclose(meta);
	return 0;
}

static void
textual_process(void)
{
	unsigned char *buffer;
	size_t written;
	size = file_load(source.filename, &buffer);
	if (!size) {
		perror(source.filename);
		exit(EXIT_FAILURE);
	}
	written = fwrite(buffer, 1, size, data);
	if (written != size) {
		perror(source.filename);
		exit(EXIT_FAILURE);
	}
	free(buffer);
	fprintf(meta, 
"const size_t %s_index = %zu;\n"
"const size_t %s_size = %zu;\n"
"\n",
		source.name, index, source.name, index + size);
	index += size;
	fclose(f);
}

static void
image_process(void)
{
	int width, height, channels;
	size_t sizep;
	unsigned char *buffer = stbi_load(source.filename, &width, &height, &channels, 0);
	if (!buffer) {
		fprintf(stderr, "%s: Error loading image\n", source.filename);
		exit(EXIT_FAILURE);
	}
	size = width * height;
	sizep = fwrite(buffer, 1, size, data);
	if (sizep != size) {
		perror(source.filename);
		exit(EXIT_FAILURE);
	}
	fprintf(meta, 
"const size_t %s_index = %zu;\n"
"const size_t %s_size = %zu;\n"
"const int %s_width = %d;\n"
"const int %s_height = %d;\n"
"const int %s_channels = %d;\n"
"\n",
		source.name, index, source.name, index + size, source.name, width, source.name, height, source.name, channels);
	index += size;
	stbi_image_free(buffer);
}

static void
font_process(void)
{
	unsigned char *buffer;
	stbtt_fontinfo font;
	float scale;
	int ascent, descent, lineGap;
	double height;
	int surface;
	int dim;
	size_t bitmap_size;
	unsigned char *bitmap = NULL;
	stbtt_pack_context ctx;
	int i;
	float x, y;
	stbtt_aligned_quad quads[95];
	stbtt_packedchar chars[95];
	if (!file_load(source_font->filename, &buffer)) {
		perror("file_load()");
		exit(EXIT_FAILURE);
	}
	if (!stbtt_InitFont(&font, buffer, 0)) {
		fprintf(stderr, "error initializing font\n");
		exit(EXIT_FAILURE);
	}
	scale = stbtt_ScaleForPixelHeight(&font, source_font->size);
	stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
	height = (ascent - descent + lineGap) * scale;
	surface = (height * 0.6) * height * 95;
	dim = (int)sqrt(surface);
	dim = pow2_next(dim);
	bitmap_size = dim * dim;
	bitmap = calloc(bitmap_size, sizeof(unsigned char));
	if (!bitmap) {
		perror(source_font->filename);
		exit(EXIT_FAILURE);
	}
	if (!stbtt_PackBegin(&ctx, bitmap, dim, dim, 0, 1, NULL)) {
		fprintf(stderr, "%s: stbtt_PackBegin\n", source_font->filename);
		exit(EXIT_FAILURE);
	}
	if (!stbtt_PackFontRange(&ctx, buffer, 0, source_font->size, 32, 95, chars)) {
		fprintf(stderr, "%s: stbtt_PackFontRange\n", source_font->filename);
		exit(EXIT_FAILURE);
	}
	free(buffer);
	stbtt_PackEnd(&ctx);
	for (i = 0; i < 95; i++) {
		x = y = 0.0f;
		stbtt_GetPackedQuad(chars, dim, dim, i, &x, &y, &quads[i], 0);
	}
	if (fwrite(bitmap, sizeof(unsigned char), bitmap_size, data) != dim * dim) {
		perror(source_font->filename);
		exit(EXIT_FAILURE);
	}
	free(bitmap);
	index += bitmap_size;
}

static size_t
file_load(const char *path, unsigned char **buffer)
{
	FILE *f;
	size_t size;
	f = fopen(path, "rb");
	if (!f) {
		return 0;
	}
	size = file_read(f, buffer);
	fclose(f);
	return size;
}

static size_t
file_read(FILE *f, unsigned char **buffer)
{
	size_t size;
	size_t readen;
	unsigned char *tmp;
	size = file_size(f);
	if (!size) {
		return 0;
	}
	*buffer = malloc(size);
	if (!*buffer) {
		return 0;
	}
	readen = fread(*buffer, 1, size, f);
	if (readen != size) {
		if (ferror(f)) {
			free(*buffer);
			*buffer = NULL;
			return 0;
		}
		tmp = realloc(*buffer, readen);
		if (!tmp) {
			free(*buffer);
			*buffer = NULL;
			return 0;
		}
		*buffer = tmp;
	}
	return readen;
}

static size_t
file_size(FILE *f)
{
	long size;
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	if (size < 0) {
		return 0;
	}
	rewind(f);
	return size;
}

static int
pow2_next(int a)
{
	int b = a;
	b--;
	b |= b >> 1;
	b |= b >> 2;
	b |= b >> 4;
	b |= b >> 8;
	b |= b >> 16;
	b++;
	return b;
}