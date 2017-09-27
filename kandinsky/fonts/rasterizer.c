/* The font rasterizer takes a standard font and converts it into a bitmap
 * embeddable font. In other words, it takes a .ttf in and outputs a .c/.h pair.
 *
 * It can also print a PNG file showing how each glyph has been rendered.
*
 * Usage: rasterizer font_name glyph_width glyph_height
 * -> Generates a .png image with the font rasterized
 * -> Generates a .c file with the content of the font */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "unicode_for_symbol.h"

#define ENSURE(action, description...) { if (!(action)) { fprintf(stderr, "Error: "); fprintf(stderr, description); fprintf(stderr, "\n"); exit(-1);}}

// Pixel format is RGB888

typedef struct {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
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

#define CHARACTER_RANGE_START 0x20
#define CHARACTER_RANGE_END 0x7E

#define GRID_WIDTH 19
#define GRID_HEIGHT 8

#if (GRID_WIDTH*GRID_HEIGHT < (NUMBER_OF_SYMBOLS+CHARACTER_RANGE_END-CHARACTER_RANGE_START+1))
#error Grid too small. Consider increasing GRID_WIDTH or GRID_HEIGHT
#endif

void drawGlyphInImage(FT_Bitmap * glyphBitmap, image_t * image, int x, int y);

int main(int argc, char * argv[]) {
  FT_Library library;
  FT_Face face;
  image_t bitmap_image;

  int expectedNumberOfArguments = 7;
#ifdef GENERATE_PNG
  expectedNumberOfArguments = 8;
#endif
  if (argc != expectedNumberOfArguments) {
#ifdef GENERATE_PNG
    fprintf(stderr, "Usage: %s font_file glyph_width glyph_height font_name output_header output_implementation output_png\n", argv[0]);
#else
    fprintf(stderr, "Usage: %s font_file glyph_width glyph_height font_name output_header output_implementation\n", argv[0]);
#endif
    fprintf(stderr, "  font_file: Path of the font file to load\n");
    fprintf(stderr, "  glyph_width: Width of bitmap glyphs, in pixels\n");
    fprintf(stderr, "  glyph_height: Height of bitmap glyphs, in pixels\n");
    fprintf(stderr, "  font_name: name of the loaded font\n");
    fprintf(stderr, "  output_header: Name of the generated C header file\n");
    fprintf(stderr, "  output_implementation: Name of the generated C source file\n");
#ifdef GENERATE_PNG
    fprintf(stderr, "  output_png: Name of the generated PNG file\n");
#endif
    exit(1);
  }

  char * font_file = argv[1];
  int requested_glyph_width = atoi(argv[2]);
  int requested_glyph_height = atoi(argv[3]);
  char * font_name = argv[4];
  char * output_header = argv[5];
  char * output_implementation = argv[6];
#ifdef GENERATE_PNG
  char * output_png = argv[7];
#endif

  ENSURE(!FT_Init_FreeType(&library), "Initializing library");

  // 0 means we're picking the first face in the provided file
  ENSURE(!FT_New_Face(library, font_file, 0, &face), "Loading font file %s", font_file);

  ENSURE(!FT_Set_Pixel_Sizes(face, requested_glyph_width, requested_glyph_height), "Setting face pixel size to %dx%d", requested_glyph_width, requested_glyph_height);

  /* Glyph metrics are complicated. Here are some useful links:
   * https://www.freetype.org/freetype2/docs/glyphs/metrics.png
   * https://www.freetype.org/freetype2/docs/glyphs/glyphs-3.html
   * https://www.freetype.org/freetype2/docs/reference/ft2-basic_types.html#FT_Bitmap
   */
  int maxWidth = 0;
  int maxAboveBaseline = 0;
  int maxBelowBaseline = 0;
  for (unsigned char character = CHARACTER_RANGE_START; character <= CHARACTER_RANGE_END; character++) {
    ENSURE(!FT_Load_Char(face, character, FT_LOAD_RENDER), "Loading character 0x%02x", character);
    int aboveBaseline = face->glyph->bitmap_top;
    int belowBaseline = face->glyph->bitmap.rows - face->glyph->bitmap_top;
    int width = face->glyph->bitmap_left + face->glyph->bitmap.width;
    if (width > maxWidth) {
      maxWidth = width;
    }
    if (aboveBaseline > maxAboveBaseline) {
      maxAboveBaseline = aboveBaseline;
    }
    if (belowBaseline > maxBelowBaseline) {
      maxBelowBaseline = belowBaseline;
    }
  }
  for (int charIndex = 0; charIndex < NUMBER_OF_SYMBOLS; charIndex++) {
    wchar_t wideChar = codePointForSymbol[charIndex];
    ENSURE(!FT_Load_Char(face, wideChar, FT_LOAD_RENDER), "Loading character 0x%02x", wideChar);
    int aboveBaseline = face->glyph->bitmap_top;
    int belowBaseline = face->glyph->bitmap.rows - face->glyph->bitmap_top;
    int width = face->glyph->bitmap_left + face->glyph->bitmap.width;
    if (width > maxWidth) {
      maxWidth = width;
    }
    if (aboveBaseline > maxAboveBaseline) {
      maxAboveBaseline = aboveBaseline;
    }
    if (belowBaseline > maxBelowBaseline) {
      maxBelowBaseline = belowBaseline;
    }
  }

  int glyph_width = maxWidth-1;
  int glyph_height = maxAboveBaseline+maxBelowBaseline;
  //printf("Actual glyph size = %dx%d\n", glyph_width, glyph_height);

  int grid_size = 1;

  bitmap_image.width = GRID_WIDTH*glyph_width+(GRID_WIDTH-1)*grid_size;
  bitmap_image.height = GRID_HEIGHT*glyph_height+(GRID_HEIGHT-1)*grid_size;
  bitmap_image.pixels = malloc(sizeof(pixel_t)*bitmap_image.width*bitmap_image.height);
  ENSURE(bitmap_image.pixels != NULL, "Allocating bitmap image of size %dx%d at %ld bytes per pixel", bitmap_image.width, bitmap_image.height, sizeof(pixel_t));

  // Draw the grid and the background
  for (int i = 0; i<bitmap_image.width;i++) {
    for (int j = 0; j<bitmap_image.height;j++) {
      pixel_t pixel = {.red = 0xFF, .green = 0xFF, .blue = 0xFF};
      if (i%(glyph_width+grid_size) >= glyph_width || j%(glyph_height+grid_size) >= glyph_height) {
        pixel = (pixel_t){.red = 0xFF, .green = 0, .blue = 0};
      }
      *(bitmap_image.pixels + j*bitmap_image.width + i) = pixel;
    }
  }

  // We're doing the ASCII table, so characters from 0 to 255 inclusive
  for (unsigned char character = CHARACTER_RANGE_START; character <= CHARACTER_RANGE_END; character++) {
    int x = (character-CHARACTER_RANGE_START)%(GRID_WIDTH);
    int y = (character-CHARACTER_RANGE_START)/(GRID_WIDTH);
    // FT_LOAD_RENDER: Render the glyph upon load
    ENSURE(!FT_Load_Char(face, character, FT_LOAD_RENDER), "Loading character 0x%02x", character);
    //printf("Advances = %dx%d\n", face->glyph->bitmap_left, face->glyph->bitmap_top);
    drawGlyphInImage(&face->glyph->bitmap,
        &bitmap_image,
        x*(glyph_width+grid_size) + face->glyph->bitmap_left,
        y*(glyph_height+grid_size) + maxAboveBaseline - face->glyph->bitmap_top
    );
  }
  // We are now using unicode to access non-ASCII characters
  for (int charIndex = 0; charIndex < NUMBER_OF_SYMBOLS; charIndex++) {
    wchar_t wideChar = codePointForSymbol[charIndex];
    int x = (charIndex+1+CHARACTER_RANGE_END-CHARACTER_RANGE_START)%(GRID_WIDTH);
    int y = (charIndex+1+CHARACTER_RANGE_END-CHARACTER_RANGE_START)/(GRID_WIDTH);
    // FT_LOAD_RENDER: Render the glyph upon load
    ENSURE(!FT_Load_Char(face, wideChar, FT_LOAD_RENDER), "Loading character 0x%02x", wideChar);
    //printf("Advances = %dx%d\n", face->glyph->bitmap_left, face->glyph->bitmap_top);
    drawGlyphInImage(&face->glyph->bitmap,
        &bitmap_image,
        x*(glyph_width+grid_size) + face->glyph->bitmap_left,
        y*(glyph_height+grid_size) + maxAboveBaseline - face->glyph->bitmap_top
    );
  }

#if GENERATE_PNG
  writeImageToPNGFile(&bitmap_image, output_png);
#endif

  FILE * headerFile = fopen(output_header, "w");
  fprintf(headerFile, "/* Auto-generated by rasterizer */\n\n");
  fprintf(headerFile, "#define BITMAP_%s_FIRST_CHARACTER 0x%2x\n", font_name, CHARACTER_RANGE_START);
  fprintf(headerFile, "#define BITMAP_%s_LAST_CHARACTER 0x%2x\n\n", font_name, CHARACTER_RANGE_END+NUMBER_OF_SYMBOLS);
  fprintf(headerFile, "#define BITMAP_%s_CHARACTER_WIDTH %d\n", font_name, glyph_width);
  fprintf(headerFile, "#define BITMAP_%s_CHARACTER_HEIGHT %d\n\n", font_name, glyph_height);
  fprintf(headerFile, "extern const unsigned char bitmap%s[%d][%d][%d];\n", font_name, NUMBER_OF_SYMBOLS+CHARACTER_RANGE_END-CHARACTER_RANGE_START+1, glyph_height, glyph_width);
  fclose(headerFile);

  FILE * sourceFile = fopen(output_implementation, "w");
  fprintf(sourceFile, "/* Auto-generated by rasterizer */\n\n");
  fprintf(sourceFile, "const unsigned char bitmap%s[%d][%d][%d] = {\n", font_name, NUMBER_OF_SYMBOLS+CHARACTER_RANGE_END-CHARACTER_RANGE_START+1, glyph_height, glyph_width);
  for (unsigned char character = CHARACTER_RANGE_START; character <= CHARACTER_RANGE_END; character++) {
    fprintf(sourceFile, "  {\n");
    int characterX = ((character-CHARACTER_RANGE_START)%GRID_WIDTH * (glyph_width+grid_size));
    int characterY = ((character-CHARACTER_RANGE_START)/GRID_WIDTH * (glyph_height+grid_size));
    for (int y = 0; y < glyph_height; y++) {
      fprintf(sourceFile, "    {");
      for (int x = 0; x < glyph_width; x++) {
        pixel_t * pixel = (bitmap_image.pixels + (y+characterY)*bitmap_image.width + (x+characterX));
        fprintf(sourceFile, "0x%02x", 0xFF - pixel->green);
        if (x+1 < glyph_width) {
          fprintf(sourceFile, ", ");
        }
      }
      fprintf(sourceFile, "}");
      if (y+1 < glyph_height) {
        fprintf(sourceFile, ",");
      }
      fprintf(sourceFile, "\n");
    }
    fprintf(sourceFile, "  }");
    fprintf(sourceFile, ",");
    fprintf(sourceFile, "\n");
  }
  for (int charIndex = 0; charIndex < NUMBER_OF_SYMBOLS; charIndex++) {
    wchar_t wideChar = codePointForSymbol[charIndex];
    fprintf(sourceFile, "  {\n");
    int characterX = ((charIndex+1+CHARACTER_RANGE_END-CHARACTER_RANGE_START)%GRID_WIDTH * (glyph_width+grid_size));
    int characterY = ((charIndex+1+CHARACTER_RANGE_END-CHARACTER_RANGE_START)/GRID_WIDTH * (glyph_height+grid_size));
    for (int y = 0; y < glyph_height; y++) {
      fprintf(sourceFile, "    {");
      for (int x = 0; x < glyph_width; x++) {
        pixel_t * pixel = (bitmap_image.pixels + (y+characterY)*bitmap_image.width + (x+characterX));
        fprintf(sourceFile, "0x%02x", 0xFF - pixel->green);
        if (x+1 < glyph_width) {
          fprintf(sourceFile, ", ");
        }
      }
      fprintf(sourceFile, "}");
      if (y+1 < glyph_height) {
        fprintf(sourceFile, ",");
      }
      fprintf(sourceFile, "\n");
    }
    fprintf(sourceFile, "  }");
    if (charIndex < NUMBER_OF_SYMBOLS - 1) {
      fprintf(sourceFile, ",");
    }
    fprintf(sourceFile, "\n");
  }

  fprintf(sourceFile, "};\n");
  fclose(sourceFile);

  free(bitmap_image.pixels);

  return 0;
}

void drawGlyphInImage(FT_Bitmap * glyphBitmap, image_t * image, int x, int y) {
  // printf("Drawing glyph. Size is %dx%d, pitch is %d\n", glyphBitmap->width, glyphBitmap->rows, glyphBitmap->pitch);
  ENSURE(glyphBitmap->pixel_mode == FT_PIXEL_MODE_GRAY, "Checking glyph is in FT_PIXEL_MODE_GRAY");
  for (int j=0;j<glyphBitmap->rows;j++) {
    for (int i=0;i<glyphBitmap->width;i++) {
      uint8_t glyphPixel = *(glyphBitmap->buffer + j*glyphBitmap->pitch + i);
      pixel_t * currentPixelPointer = (image->pixels + (y+j)*image->width + (x+i));
      *currentPixelPointer = (pixel_t){.red = (0xFF-glyphPixel), .green = (0xFF-glyphPixel), .blue = (0xFF-glyphPixel)}; // Alpha blending
    }
  }
}

#if GENERATE_PNG

void writeImageToPNGFile(image_t * image, char * filename) {
  FILE * file = fopen(filename, "wb"); // Write in binary mode
  ENSURE(file != NULL, "Opening file %s", filename);

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  ENSURE(png != NULL, "Allocating PNG write structure");

  png_infop info = png_create_info_struct(png);
  ENSURE(info != NULL, "Allocating info structure");

  png_init_io(png, file);

  png_set_IHDR(png, info,
      image->width, image->height,
      8, // Number of bits per channel
      PNG_COLOR_TYPE_RGB,
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
