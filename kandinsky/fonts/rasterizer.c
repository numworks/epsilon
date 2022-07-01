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

#include "code_points.h"

#include "../../ion/src/external/lz4/lz4hc.h"


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
#define PNG_SKIP_SETJMP_CHECK
#include <png.h>
void writeImageToPNGFile(image_t * image, char * filename);
#endif

void drawGlyphInImage(FT_Bitmap * glyphBitmap, image_t * image, int x, int y);
static void prettyPrintArray(FILE * stream, int maxWidth, int typeSize, void * array, int numberOfElements);

int main(int argc, char * argv[]) {
  FT_Library library;
  FT_Face face;
  image_t bitmap_image;

  int expectedNumberOfArguments = 9;
#ifdef GENERATE_PNG
  expectedNumberOfArguments = 10;
#endif
  if (argc != expectedNumberOfArguments) {
#ifdef GENERATE_PNG
    fprintf(stderr, "Usage: %s font_file extended glyph_width glyph_height font_name output_cpp output_png\n", argv[0]);
#else
    fprintf(stderr, "Usage: %s font_file extended glyph_width glyph_height font_name output_cpp\n", argv[0]);
#endif
    fprintf(stderr, "  font_file: Path of the font file to load\n");
    fprintf(stderr, "  extended: 0 if simple set of code points, else 1\n");
    fprintf(stderr, "  glyph_width: Width of bitmap glyphs, in pixels\n");
    fprintf(stderr, "  glyph_height: Height of bitmap glyphs, in pixels\n");
    fprintf(stderr, "  packed_glyph_width: Minimal glyph width in pixels. Pass 0 if unsure.\n");
    fprintf(stderr, "  packed_glyph_height: Minimal glyph height in pixels. Pass 0 if unsure.\n");
    fprintf(stderr, "  font_name: name of the loaded font\n");
    fprintf(stderr, "  output_cpp: Name of the generated C source file\n");
#ifdef GENERATE_PNG
    fprintf(stderr, "  output_png: Name of the generated PNG file\n");
#endif
    exit(1);
  }

  char * font_file = argv[1];
  int extended_set = atoi(argv[2]);
  int requested_glyph_width = atoi(argv[3]);
  int requested_glyph_height = atoi(argv[4]);
  int packed_glyph_width = atoi(argv[5]);
  int packed_glyph_height = atoi(argv[6]);
  char * font_name = argv[7];
  char * output_cpp = argv[8];
#ifdef GENERATE_PNG
  char * output_png = argv[9];
#endif

  uint32_t * CodePoints = extended_set ? ExtendedCodePoints : SimpleCodePoints;
  int NumberOfCodePoints = extended_set ? NumberOfExtendedCodePoints : NumberOfSimpleCodePoints;

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
  for (int i=0; i < NumberOfCodePoints; i++) {
    wchar_t codePoint = CodePoints[i];
    ENSURE(!FT_Load_Char(face, codePoint, FT_LOAD_RENDER), "Loading character 0x%02x", codePoint);
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
    // printf("Codepoint %04x : %dx%d\n", codePoint, width, aboveBaseline+belowBaseline);
  }

  int glyph_width = maxWidth-1;
  if (glyph_width == 9) {  // FIXME: This is a TEMPORARY FIX, we should instead fix the ttf file
    glyph_width += 1;
  } else if (glyph_width == 8) {
    glyph_width -= 1;
  }
  if (packed_glyph_width != 0) {
    ENSURE(glyph_width == packed_glyph_width, "Expecting a packed glyph width of %d but got %d instead", packed_glyph_width, glyph_width);
  } else {
    printf("Computed packed_glyph_width = %d\n", glyph_width);
  }
  int glyph_height = maxAboveBaseline+maxBelowBaseline;
  if (glyph_height == 13) { // FIXME: Same here
    glyph_height += 1;
  }
  if (packed_glyph_height != 0) {
    ENSURE(glyph_height == packed_glyph_height, "Expecting a packed glyph height of %d but got %d instead", packed_glyph_height, glyph_height);
  } else {
    printf("Computed packed_glyph_height = %d\n", glyph_height);
  }

  int grid_size = 1;
  int grid_width = 20;
  int grid_height = ((NumberOfCodePoints-1)/grid_width)+1;

  bitmap_image.width = grid_width*glyph_width+(grid_width-1)*grid_size;
  bitmap_image.height = grid_height*glyph_height+(grid_height-1)*grid_size;
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

  for (int i=0; i<NumberOfCodePoints; i++) {
    wchar_t codePoint = CodePoints[i];
    int x = i%grid_width;
    int y = i/grid_width;
    // FT_LOAD_RENDER: Render the glyph upon load
    ENSURE(!FT_Load_Char(face, codePoint, FT_LOAD_RENDER), "Loading character 0x%08x", codePoint);
    //printf("Advances = %dx%d\n", face->glyph->bitmap_left, face->glyph->bitmap_top);
    while (face->glyph->bitmap_left < 0) {
      // This is a workaround for combining glyphs.
      // For some reason, FreeType does a fun hack and yields a negative bitmap_left
      // This way, the glyph automagically combines with the previous one. That's neat,
      // but we don't want to do that.
      face->glyph->bitmap_left += glyph_width;
    }
    drawGlyphInImage(&face->glyph->bitmap,
        &bitmap_image,
        x*(glyph_width+grid_size)  + face->glyph->bitmap_left,
        y*(glyph_height+grid_size) + maxAboveBaseline - face->glyph->bitmap_top
    );
  }

#if GENERATE_PNG
  writeImageToPNGFile(&bitmap_image, output_png);
#endif

  FILE * sourceFile = fopen(output_cpp, "w");

  fprintf(sourceFile, "/* This file is auto-generated by the rasterizer */\n\n");
  fprintf(sourceFile, "#include <kandinsky/font.h>\n\n");

  // Step 1 - Build the GlyphIndex <-> UnicodeCodePoint correspondence table

  int previousIndex = -1;
  uint32_t previousCodePoint = 0;
  int numberOfPairs = 0;

  fprintf(sourceFile, "static constexpr KDFont::CodePointIndexPair table[] = {\n");
  for (int i=0; i<NumberOfCodePoints; i++) {
    int currentCodePoint = CodePoints[i];
    if (currentCodePoint != (previousCodePoint + (i-previousIndex))) {
      fprintf(sourceFile, "  KDFont::CodePointIndexPair(0x%x, %d),\n", currentCodePoint, i);
      previousCodePoint = currentCodePoint;
      previousIndex = i;
      numberOfPairs++;
    }
  }
  fprintf(sourceFile, "};\n\n");

  fprintf(sourceFile, "static constexpr size_t tableLength = %d;\n\n", numberOfPairs);

  // Step 2 - Build Glyph data

  fprintf(sourceFile, "static constexpr KDCoordinate glyphWidth = %d;\n\n", glyph_width);
  fprintf(sourceFile, "static constexpr KDCoordinate glyphHeight = %d;\n\n", glyph_height);

  int grayscaleBitsPerPixel = 4;

  int sizeOfUncompressedGlyphBuffer = glyph_width * glyph_height * grayscaleBitsPerPixel/8;
  ENSURE(8*sizeOfUncompressedGlyphBuffer == glyph_width * glyph_height * grayscaleBitsPerPixel, "Error: the glyph size (%dx%d@%dbpp) cannot fit in an integral number of bytes", glyph_width, glyph_height, grayscaleBitsPerPixel);
  uint8_t * uncompressedGlyphBuffer = (uint8_t *)malloc(sizeOfUncompressedGlyphBuffer);

  uint16_t glyphDataOffset[NumberOfCodePoints+1];
  int maxGlyphDataSize = NumberOfCodePoints* sizeOfUncompressedGlyphBuffer;
  uint8_t * glyphData = (uint8_t *)malloc(maxGlyphDataSize);
  uint16_t lastOffset = 0;

  for (int character = 0; character < NumberOfCodePoints; character++) {
    int characterX = (character%grid_width * (glyph_width+grid_size));
    int characterY = (character/grid_width * (glyph_height+grid_size));
    uint8_t accumulator = 0;
    int uncompressedGlyphBufferIndex = 0;
    int numberOfValuesAccumulated = 0;
    for (int y = 0; y < glyph_height; y++) {
      for (int x = 0; x < glyph_width; x++) {
        pixel_t * pixel = (bitmap_image.pixels + (y+characterY)*bitmap_image.width + (x+characterX));

        uint8_t grayscaleValue = (0xFF - pixel->green) >> (8 - grayscaleBitsPerPixel);
        accumulator = (accumulator << grayscaleBitsPerPixel) | grayscaleValue;
        if (numberOfValuesAccumulated++ == (8/grayscaleBitsPerPixel)-1) {
          uncompressedGlyphBuffer[uncompressedGlyphBufferIndex++] = accumulator;
          accumulator = 0;
          numberOfValuesAccumulated = 0;
        }
      }
    }
    ENSURE(accumulator == 0, "Discarded accumulator data (accumulator = %d)", accumulator);
    ENSURE(numberOfValuesAccumulated == 0, "Discarded accumulator data (numberOfValuesAccumulated = %d)", numberOfValuesAccumulated);
    ENSURE(uncompressedGlyphBufferIndex == sizeOfUncompressedGlyphBuffer, "Error filling uncompressed buffer, only %d out of %d", uncompressedGlyphBufferIndex, sizeOfUncompressedGlyphBuffer);

    int sizeOfCompressedGlyphBuffer = LZ4_compress_HC(
      (const char *)uncompressedGlyphBuffer,
      (char *)(glyphData + lastOffset),
      sizeOfUncompressedGlyphBuffer,
      maxGlyphDataSize - lastOffset,
      LZ4HC_CLEVEL_MAX
    );

    ENSURE(sizeOfCompressedGlyphBuffer > 0, "Could not compress glyph %d", character);

    glyphDataOffset[character] = lastOffset;
    lastOffset += sizeOfCompressedGlyphBuffer;
  }
  glyphDataOffset[NumberOfCodePoints] = lastOffset;

  fprintf(sourceFile, "static constexpr uint16_t glyphDataOffset[%d] = {", NumberOfCodePoints+1);
  prettyPrintArray(sourceFile, 80, 2, glyphDataOffset, NumberOfCodePoints+1);
  fprintf(sourceFile, "};\n\n");

  size_t finalDataSize = lastOffset;
  size_t initialDataSize = NumberOfCodePoints * glyph_width * glyph_height;

  fprintf(sourceFile, "/* Rasterized  = %5zu bytes (%d glyphs x %d pixels)\n", initialDataSize, NumberOfCodePoints, glyph_width*glyph_height);
  fprintf(sourceFile, " * Downsampled = %5lu bytes (1/%d of rasterized)\n", initialDataSize*grayscaleBitsPerPixel/8, 8/grayscaleBitsPerPixel);
  fprintf(sourceFile, " * Compressed  = %5zu bytes (%.2f%% of rasterized) */\n", finalDataSize, 100.0*finalDataSize/initialDataSize);

  fprintf(sourceFile, "static constexpr uint8_t glyphData[%d] = {", lastOffset);
  prettyPrintArray(sourceFile, 80, 1, glyphData, lastOffset);
  fprintf(sourceFile, "};\n\n");

  free(glyphData);
  free(uncompressedGlyphBuffer);

  fprintf(sourceFile, "const KDFont KDFont::private%s(tableLength, table, glyphWidth, glyphHeight, glyphDataOffset, glyphData);\n", font_name);

  fclose(sourceFile);

  free(bitmap_image.pixels);

  return 0;
}

static void prettyPrintArray(FILE * stream, int maxWidth, int typeSize, void * array, int numberOfElements) {
  int lineWidth = 999;
  for (int i=0; i<numberOfElements; i++) {
    int itemWidth = 1 + 2*typeSize + 2 + 1; // space + 0x + data + comma
    if (lineWidth + itemWidth > maxWidth) {
      fprintf(stream, "\n ");
      lineWidth = 1;
    }
    if (typeSize == 1) {
      fprintf(stream, " 0x%02x", ((uint8_t *)array)[i]);
    } else if (typeSize == 2) {
      fprintf(stream, " 0x%04x", ((uint16_t *)array)[i]);
    }
    if (i == numberOfElements-1) {
      fprintf(stream, "\n");
    } else {
      fprintf(stream, ",");
    }
    lineWidth += itemWidth;
  }
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
