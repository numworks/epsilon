/* The font rasterizer takes a standard font and converts it into a bitmap
 * embeddable font. In other words, it takes a .ttf in and outputs a .c/.h pair.
 *
 * It can also print a PNG file showing how each glyph has been rendered.
 *
 * Usage: rasterizer fontFile requestedGlyphWidth requestedGlyphHeight
 * packedGlyphWidth packedGlyphHeight fontName outputH outputCpp sharedFontPath
 * outputPng
 * -> Generates a .png image with the font rasterized
 * -> Generates a .c and a .h files with the content of the font */

#ifndef KANDINSKY_FONTS_RASTERIZER_H
#define KANDINSKY_FONTS_RASTERIZER_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "code_points.h"
#include "font_constants.h"

#define ENSURE(action, description...) \
  {                                    \
    if (!(action)) {                   \
      fprintf(stderr, "Error: ");      \
      fprintf(stderr, description);    \
      fprintf(stderr, "\n");           \
      exit(-1);                        \
    }                                  \
  }

int max(int a, int b) { return a < b ? b : a; }

void checkInputs(int argc, char* argv[]);
void initTTF(FT_Library* library, FT_Face* face, const char* font_file,
             int requestedGlyphWidth, int requestedGlyphHeight);
void computeMetrics(FT_Face face, int* maxWidth, int* maxAboveBaseline,
                    int* maxBelowBaseline);
void checkGlyphDimensions(int glyphWidth, int requestedGlyphWidth,
                          int glyphHeight, int requestedGlyphHeight);
int writeCodePointToGlyphTableFiles(const char* cppFilename,
                                    int grayscaleBitsPerPixel,
                                    int maxGlyphPixelCount);
void generateGlyphData(FT_Face face, uint16_t* glyphDataOffset,
                       int* glyphDataOffsetLength, uint8_t* glyphData,
                       int sizeOfGlyphData, int* glyphDataLength,
                       int glyphWidth, int glyphHeight, int maxAboveBaseline,
                       int grayscaleBitsPerPixel);
void writeFontHeaderFile(const char* fontHeaderFilename, const char* fontName,
                         int glyphWidth, int glyphHeight);
void writeFontSourceFile(const char* fontSourceFilename, const char* fontName,
                         int glyphWidth, int glyphHeight,
                         uint16_t* glyphDataOffset, int glyphDataOffsetLength,
                         uint8_t* glyphData, int glyphDataLength,
                         int grayscaleBitsPerPixel,
                         int CodePointToGlyphIndexLength);
void storeRenderedGlyphsImage(const char* outputImage, FT_Face face,
                              int glyphWidth, int glyphHeight,
                              int maxAboveBaseline);

void loadCodePoint(FT_Face face, wchar_t codePoint,
                   int combinedCharactersShift);
void drawGlyphOnBuffer(FT_GlyphSlot glyph, int glyphWidth, int glyphHeight,
                       int maxAboveBaseline, uint8_t* glyphBuffer,
                       int sizeOfUncompressedGlyphBuffer,
                       int grayscaleBitsPerPixel);
int compressAndAppend(uint8_t* uncompressedGlyphBuffer,
                      int sizeOfUncompressedGlyphBuffer, uint8_t* glyphData,
                      int glyphDataOffset, int maxGlyphDataSize);
int writeCodePointIndexPairTable(FILE* output);

static void prettyPrintArray(FILE* stream, int maxWidth, int typeSize,
                             void* array, int numberOfElements);

#ifdef GENERATE_PNG
#define PNG_SKIP_SETJMP_CHECK
#include <png.h>

// Pixel format is RGB888
typedef struct {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
} pixel_t;

typedef struct {
  pixel_t* pixels;
  int width;
  int height;
} image_t;

void drawBackground(image_t* bitmap_image, int glyphWidth, int glyphHeight,
                    int grid_size, int maxAboveBaseline);
void drawGlyphInImage(FT_Bitmap* glyphBitmap, image_t* image, int xOffset,
                      int yOffset);
void drawAllGlyphsInImage(FT_Face face, image_t* bitmap_image, int gridWidth,
                          int glyphWidth, int glyphHeight, int grid_size,
                          int maxAboveBaseline);
void writeImageToPNGFile(image_t* image, const char* filename);
#endif

#endif
