/* example1.c                                                      */
/*                                                                 */
/* This small program shows how to print a rotated string with the */
/* FreeType 2 library.                                             */

/*
 * Usage: rasterizer font_name glyph_width glyph_height
 * -> Generates a .png image with the font rasterized
 * -> Generates a .c file with the content of the font
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define ENSURE(action, description...) { if (!(action)) { fprintf(stderr, "Error: "); fprintf(stderr, description); fprintf(stderr, "\n"); exit(-1);}}

// Pixel format is BGRA8888

typedef struct {
  char blue;
  char green;
  char red;
  char alpha;
} pixel_t;

typedef struct {
  pixel_t * pixels;
  int width;
  int height;
} image_t;

#ifdef GENERATE_PNG
#include <png.h>
void writeImageToPNGFile(image_t * image, char * filename);
#endif


void drawGlyphInImage(FT_Bitmap * glyphBitmap, image_t * image, int x, int y);

int main(int argc, char * argv[]) {
  FT_Library library;
  FT_Face face;
  image_t bitmap_image;

  if (argc != 4) {
    fprintf(stderr, "Usage: %s font_file glyph_width glyph_height\n", argv[0]);
    fprintf(stderr, "  font_file: Path of the font file to load\n");
    fprintf(stderr, "  glyph_width: Width of bitmap glyphs, in pixels\n");
    fprintf(stderr, "  glyph_height: Height of bitmap glyphs, in pixels\n");
    exit(1);
  }

  char * font_file = argv[1];
  int glyph_width = atoi(argv[2]);
  int glyph_height = atoi(argv[3]);

  int grid_size = 1;

  bitmap_image.width = 16*glyph_width+15*grid_size;
  bitmap_image.height = 16*glyph_height+15*grid_size;
  bitmap_image.pixels = malloc(sizeof(pixel_t)*bitmap_image.width*bitmap_image.height);
  ENSURE(bitmap_image.pixels != NULL, "Allocating bitmap image of size %dx%d at %ld bytes per pixel", glyph_width*16, glyph_height*16, sizeof(pixel_t));

  // Draw the grid
  for (int i = 0; i<bitmap_image.width;i++) {
    for (int j = 0; j<bitmap_image.height;j++) {
      if (i%(glyph_width+grid_size) >= glyph_width || j%(glyph_height+grid_size) >= glyph_height) {
        *(bitmap_image.pixels + j*bitmap_image.width + i) = (pixel_t){.red = 0xFF, .green = 0, .blue = 0, .alpha = 0xFF};
      }
    }
  }

  ENSURE(!FT_Init_FreeType(&library), "Initializing library");

  // 0 means we're picking the first face in the provided file
  ENSURE(!FT_New_Face(library, font_file, 0, &face), "Loading font file %s", font_file);

  ENSURE(!FT_Set_Pixel_Sizes(face, glyph_width, glyph_height), "Setting face pixel size to %dx%d", glyph_width, glyph_height);

  // We're doing the ASCII table, so characters from 0 to 255 inclusive
  for (int i=0; i<256; i++) {
    unsigned char character = (unsigned char)i;
    int x = character%16;
    int y = character/16;
    // FT_LOAD_RENDER: Render the glyph upon load
    ENSURE(!FT_Load_Char(face, character, FT_LOAD_RENDER), "Loading character 0x%02x", character);
    drawGlyphInImage(&face->glyph->bitmap, &bitmap_image, x*(glyph_width+grid_size), y*(glyph_height+grid_size));
  }

#if GENERATE_PNG
  writeImageToPNGFile(&bitmap_image, "out.png");
#endif

  free(bitmap_image.pixels);

  return 0;
}

void drawGlyphInImage(FT_Bitmap * glyphBitmap, image_t * image, int x, int y) {
  // printf("Drawing glyph. Size is %dx%d, pitch is %d\n", glyphBitmap->width, glyphBitmap->rows, glyphBitmap->pitch);
  ENSURE(glyphBitmap->pixel_mode == FT_PIXEL_MODE_GRAY, "Checking glyph is in FT_PIXEL_MODE_GRAY");
  for (int j=0;j<glyphBitmap->rows;j++) {
    for (int i=0;i<glyphBitmap->width;i++) {
      uint8_t glyphPixel = *(glyphBitmap->buffer + j*glyphBitmap->pitch + i);
      *(image->pixels + (y+j)*image->width + (x+i)) = (pixel_t){.alpha = glyphPixel};
    }
  }
}

#if GENERATE_PNG

void writeImageToPNGFile(image_t * image, char * filename) {
  FILE * file = fopen(filename, "wb"); // Write in binary mode
  ENSURE(file != NULL, "Opening file %s", filename);

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  ENSURE(png != NULL, "Allocating PNG write structure");

  png_set_bgr(png); // Our pixel_t is BGRA

  png_infop info = png_create_info_struct(png);
  ENSURE(info != NULL, "Allocating info structure");

  png_init_io(png, file);

  png_set_IHDR(png, info,
      image->width, image->height,
      8, // Number of bits per channel
      PNG_COLOR_TYPE_RGB_ALPHA, // RGBA
      PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT,
      PNG_FILTER_TYPE_DEFAULT);

  png_write_info(png, info);

  for (int j=0;j<image->height;j++) {
    png_write_row(png, (png_bytep)(image->pixels+j*image->width));
  }

  png_write_end(png, NULL);

  png_free_data(png, info, PNG_FREE_ALL, -1); // -1 = all items
  png_destroy_write_struct(&png, (png_infopp)NULL);
  fclose(file);
}
#endif
