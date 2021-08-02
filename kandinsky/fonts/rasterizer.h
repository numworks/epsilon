/* The font rasterizer takes a standard font and converts it into a bitmap
 * embeddable font. In other words, it takes a .ttf in and outputs a .c/.h pair.
 *
 * It can also print a PNG file showing how each glyph has been rendered.
 *
 * Usage: rasterizer font_file glyph_width glyph_height font_name output_cpp output_png
 * -> Generates a .png image with the font rasterized
 * -> Generates a .c file with the content of the font */

#ifndef KANDINSKY_FONTS_RASTERIZER_H
#define KANDINSKY_FONTS_RASTERIZER_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "code_points.h"

#define ENSURE(action, description...) \
  {                                    \
    if (!(action)) {                   \
      fprintf(stderr, "Error: ");      \
      fprintf(stderr, description);    \
      fprintf(stderr, "\n");           \
      exit(-1);                        \
    }                                  \
  }

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


int max(int a, int b) {
  return a < b ? b : a;
}
void computeBBox(FT_Face face, int * maxWidth, int * maxAboveBaseline, int * maxBelowBaseline);
void drawGridAndBackground(image_t * bitmap_image,
                           int glyph_width,
                           int glyph_height,
                           int grid_size,
                           int maxAboveBaseline);
void drawAllGlyphsInImage(FT_Face face,
                          image_t * bitmap_image,
                          int grid_width,
                          int glyph_width,
                          int glyph_height,
                          int grid_size,
                          int maxAboveBaseline);
void generateSourceFile(const char * filepath,
                        image_t * bitmap_image,
                        int grid_width,
                        int glyph_width,
                        int glyph_height,
                        int grid_size,
                        const char * font_name);
void writeCodePointIndexPairTable(FILE * output);
void fillGlyphBuffer(uint8_t * uncompressedGlyphBuffer,
                     int codepoint,
                     image_t * bitmap_image,
                     int grid_width,
                     int glyph_width,
                     int glyph_height,
                     int grid_size,
                     int grayscaleBitsPerPixel,
                     int sizeOfUncompressedGlyphBuffer);
void writeGlyphData(FILE * output,
                    image_t * bitmap_image,
                    int grid_width,
                    int glyph_width,
                    int glyph_height,
                    int grid_size);

#ifdef GENERATE_PNG
#define PNG_SKIP_SETJMP_CHECK
#include <png.h>
void writeImageToPNGFile(image_t * image, char * filename);
#endif

void drawGlyphInImage(FT_Bitmap * glyphBitmap, image_t * image, int x, int y);
static void prettyPrintArray(FILE * stream,
                             int maxWidth,
                             int typeSize,
                             void * array,
                             int numberOfElements);

#endif /* KANDINSKY_FONTS_RASTERIZER_H */
