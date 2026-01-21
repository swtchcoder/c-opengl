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

static FILE *data;
static FILE *meta;
static const long textual_count = sizeof(textuals) / sizeof(source_t);
static source_t source;
static const font_t *source_font;
static FILE *f;
static const long image_count = sizeof(images) / sizeof(source_t);
static const long font_count = sizeof(fonts) / sizeof(font_t);
static long size;
static long index = 0;

int
main(void)
{
	long i;
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
	char *buffer;
	long sizep;
	f = fopen(source.filename, "r");
	if (!f) {
		perror(source.filename);
		exit(1);
	}
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	rewind(f);
	buffer = malloc(size);
	if (!buffer) {
		perror(source.filename);
		exit(1);
	}
	sizep = fread(buffer, 1, size, f);
	if (sizep != size) {
		if (ferror(f)) {
			perror(source.filename);
			exit(1);
		}
		size = sizep;
		buffer = realloc(buffer, size);
		if (!buffer) {
			perror(source.filename);
			exit(1);
		}
	}
	sizep = fwrite(buffer, 1, size, data);
	if (sizep != size) {
		perror(source.filename);
		exit(1);
	}
	free(buffer);
	fprintf(meta, 
"long %s_index = %ld;\n"
"long %s_size = %ld;\n"
"\n",
		source.name, index, source.name, index + size);
	index += size;
	fclose(f);
}

static void
image_process(void)
{
	int width, height, channels;
	long sizep;
	unsigned char *buffer = stbi_load(source.filename, &width, &height, &channels, 0);
	if (!buffer) {
		fprintf(stderr, "%s: Error loading image\n", source.filename);
		exit(1);
	}
	size = width * height;
	sizep = fwrite(buffer, 1, size, data);
	if (sizep != size) {
		perror(source.filename);
		exit(1);
	}
	fprintf(meta, 
"long %s_index = %ld;\n"
"long %s_size = %ld;\n"
"int %s_width = %d;\n"
"int %s_height = %d;\n"
"int %s_channels = %d;\n"
"\n",
		source.name, index, source.name, index + size, source.name, width, source.name, height, source.name, channels);
	index += size;
	stbi_image_free(buffer);
}

static void
font_process(void)
{
	FILE *file;
	long size;
	unsigned char *buffer;
	size_t buffer_size;
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
	file = fopen(source_font->filename, "rb");
	if (!file) {
		perror(source_font->filename);
		exit(1);
	}
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	if (size < 0) {
		perror("ftell");
		exit(EXIT_FAILURE);
	}
	rewind(file);
	buffer = malloc(size + 1);
	if (!buffer) {
		perror(source_font->filename);
		exit(1);
	}
	buffer_size = fread(buffer, 1, size, file);
	if (buffer_size != (size_t)size) {
		if (ferror(file)) {
			perror(source_font->filename);
			exit(1);
		}
		buffer = realloc(buffer, buffer_size + 1);
		if (!buffer) {
			perror(source_font->filename);
			exit(1);
		}
	}
	fclose(file);
	buffer[buffer_size] = 0;
	if (!stbtt_InitFont(&font, buffer, 0)) {
		fprintf(stderr, "error initializing font\n");
		exit(EXIT_FAILURE);
	}
	scale = stbtt_ScaleForPixelHeight(&font, source_font->size);
	stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
	height = (ascent - descent + lineGap) * scale;
	surface = (height * 0.6) * height * 95;
	dim = (int)sqrt(surface);
	dim--;
	dim |= dim >> 1;
	dim |= dim >> 2;
	dim |= dim >> 4;
	dim |= dim >> 8;
	dim |= dim >> 16;
	dim++;
	bitmap_size = dim * dim;
	bitmap = calloc(bitmap_size, sizeof(unsigned char));
	if (!bitmap) {
		perror(source_font->filename);
		exit(1);
	}
	if (!stbtt_PackBegin(&ctx, bitmap, dim, dim, 0, 1, NULL)) {
		fprintf(stderr, "%s: stbtt_PackBegin\n", source_font->filename);
		exit(1);
	}
	if (!stbtt_PackFontRange(&ctx, buffer, 0, source_font->size, 32, 95, chars)) {
		fprintf(stderr, "%s: stbtt_PackFontRange\n", source_font->filename);
		exit(1);
	}
	free(buffer);
	stbtt_PackEnd(&ctx);
	for (i = 0; i < 95; i++) {
		x = y = 0.0f;
		stbtt_GetPackedQuad(chars, dim, dim, i, &x, &y, &quads[i], 0);
	}

	if (fwrite(bitmap, sizeof(unsigned char), bitmap_size, data) != dim * dim) {
		perror(source_font->filename);
		exit(1);
	}
	free(bitmap);
	index += bitmap_size;
}