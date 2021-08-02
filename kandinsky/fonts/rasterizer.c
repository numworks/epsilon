#include "rasterizer.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../../ion/src/external/lz4/lz4hc.h"

int main(int argc, char * argv[]) {
  FT_Library library;
  FT_Face face;
  image_t bitmap_image;

  int expectedNumberOfArguments = 8;
#ifdef GENERATE_PNG
  expectedNumberOfArguments = 9;
#endif
  if (argc != expectedNumberOfArguments) {
#ifdef GENERATE_PNG
    fprintf(stderr, "Usage: %s font_file glyph_width glyph_height font_name output_cpp output_png\n", argv[0]);
#else
    fprintf(stderr, "Usage: %s font_file glyph_width glyph_height font_name output_cpp\n", argv[0]);
#endif
    fprintf(stderr, "  font_file: Path of the font file to load\n");
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
  int requested_glyph_width = atoi(argv[2]);
  int requested_glyph_height = atoi(argv[3]);
  int packed_glyph_width = atoi(argv[4]);
  int packed_glyph_height = atoi(argv[5]);
  char * font_name = argv[6];
  char * output_cpp = argv[7];
#ifdef GENERATE_PNG
  char * output_png = argv[8];
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
  computeBBox(face, &maxWidth, &maxAboveBaseline, &maxBelowBaseline);
  ENSURE(maxWidth > 0 && maxAboveBaseline > 0 && maxBelowBaseline > 0, "Computing bounds");

  int glyph_width = maxWidth - 1;  // TODO remove the -1
  if (packed_glyph_width != 0) {
    ENSURE(glyph_width == packed_glyph_width, "Expecting a packed glyph width of %d but got %d instead", packed_glyph_width, glyph_width);
  } else {
    printf("Computed packed_glyph_width = %d\n", glyph_width);
  }
  int glyph_height = maxAboveBaseline + maxBelowBaseline;
  if (packed_glyph_height != 0) {
    ENSURE(glyph_height == packed_glyph_height, "Expecting a packed glyph height of %d but got %d instead", packed_glyph_height, glyph_height);
  } else {
    printf("Computed packed_glyph_height = %d\n", glyph_height);
  }

  int grid_size = 1;
  int grid_width = 20;
  int grid_height = ((NumberOfCodePoints - 1) / grid_width) + 1;

  bitmap_image.width = grid_width * (glyph_width + grid_size) - 1 * grid_size;
  bitmap_image.height = grid_height * (glyph_height + grid_size) - 1 * grid_size;
  bitmap_image.pixels = malloc(sizeof(pixel_t) * bitmap_image.width * bitmap_image.height);
  ENSURE(bitmap_image.pixels != NULL, "Allocating bitmap image of size %dx%d at %ld bytes per pixel", bitmap_image.width, bitmap_image.height, sizeof(pixel_t));

  drawGridAndBackground(&bitmap_image, glyph_width, glyph_height, grid_size, maxAboveBaseline);
  drawAllGlyphsInImage(face, &bitmap_image, grid_width, glyph_width, glyph_height, grid_size, maxAboveBaseline);

#if GENERATE_PNG
  writeImageToPNGFile(&bitmap_image, output_png);
#endif

  generateSourceFile(output_cpp, &bitmap_image, grid_width, glyph_width, glyph_height, grid_size, font_name);

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

void computeBBox(FT_Face face, int * maxWidth, int * maxAboveBaseline, int * maxBelowBaseline) {
  for (int i=0; i < NumberOfCodePoints; i++) {
    wchar_t codePoint = CodePoints[i];
    // Load codePoint into face->glyph
    ENSURE(!FT_Load_Char(face, codePoint, FT_LOAD_RENDER), "Loading character 0x%02x", codePoint);
    int aboveBaseline = face->glyph->bitmap_top;
    int belowBaseline = face->glyph->bitmap.rows - face->glyph->bitmap_top;
    int width = face->glyph->bitmap_left + face->glyph->bitmap.width;
    *maxWidth = max(width, *maxWidth);
    *maxAboveBaseline = max(aboveBaseline, *maxAboveBaseline);
    *maxBelowBaseline = max(belowBaseline, *maxBelowBaseline);
    // printf("Codepoint %04x : %dx%d\n", codePoint, width, aboveBaseline+belowBaseline);
  }
}

void drawGridAndBackground(image_t * bitmap_image, int glyph_width, int glyph_height, int grid_size, int maxAboveBaseline) {
  pixel_t grid_color = {.red = 0xFF, .green = 0, .blue = 0};
  pixel_t baseline_color = {.red = 0x82, .green = 0xFF, .blue = 0x82};
  pixel_t white = {.red = 0xFF, .green = 0xFF, .blue = 0xFF};
  for (int i = 0; i < bitmap_image->width; i++) {
    for (int j = 0; j < bitmap_image->height; j++) {
      pixel_t pixel;
      if (i % (glyph_width + grid_size) >= glyph_width ||
          j % (glyph_height + grid_size) >= glyph_height) {
        pixel = grid_color;
      } else if (j % (glyph_height + grid_size) == maxAboveBaseline) {
        pixel = baseline_color;
      } else {
        pixel = white;
      }
      bitmap_image->pixels[j * bitmap_image->width + i] = pixel;
    }
  }
}

void drawAllGlyphsInImage(FT_Face face,
                          image_t * bitmap_image,
                          int grid_width,
                          int glyph_width,
                          int glyph_height,
                          int grid_size,
                          int maxAboveBaseline) {
  for (int i = 0; i < NumberOfCodePoints; i++) {
    wchar_t codePoint = CodePoints[i];
    int x = i % grid_width;
    int y = i / grid_width;
    // FT_LOAD_RENDER: Render the glyph upon load
    ENSURE(!FT_Load_Char(face, codePoint, FT_LOAD_RENDER), "Loading character 0x%08x", codePoint);
    // printf("Advances = %dx%d\n", face->glyph->bitmap_left, face->glyph->bitmap_top);
    if (face->glyph->bitmap_left < 0) {
      // Combining characters are shifted left in FreeType so as to combine with the previous
      // character We don't want that, so we move it back right
      face->glyph->bitmap_left += glyph_width;
    }
    drawGlyphInImage(&face->glyph->bitmap,
                     bitmap_image,
                     x * (glyph_width + grid_size) + face->glyph->bitmap_left,
                     y * (glyph_height + grid_size) + maxAboveBaseline - face->glyph->bitmap_top);
  }
}

void generateSourceFile(const char * filepath,
                        image_t * bitmap_image,
                        int grid_width,
                        int glyph_width,
                        int glyph_height,
                        int grid_size,
                        const char * font_name) {
  FILE * sourceFile = fopen(filepath, "w");

  fprintf(sourceFile, "/* This file is auto-generated by the rasterizer */\n\n");
  fprintf(sourceFile, "#include <kandinsky/font.h>\n\n");

  // Step 1 - Build the GlyphIndex <-> UnicodeCodePoint correspondance table
  writeCodePointIndexPairTable(sourceFile);

  // Step 2 - Build Glyph data
  writeGlyphData(sourceFile, bitmap_image, grid_width, glyph_width, glyph_height, grid_size);

  fprintf(sourceFile, "const KDFont KDFont::private%s(tableLength, table, glyphWidth, glyphHeight, glyphDataOffset, glyphData);\n", font_name);

  fclose(sourceFile);
}

void writeCodePointIndexPairTable(FILE * output) {
  int previousIndex = -1;
  uint32_t previousCodePoint = 0;
  int numberOfPairs = 0;

  fprintf(output, "static constexpr KDFont::CodePointIndexPair table[] = {\n");
  for (int i=0; i<NumberOfCodePoints; i++) {
    int currentCodePoint = CodePoints[i];
    if (currentCodePoint != (previousCodePoint + (i-previousIndex))) {
      fprintf(output, "  KDFont::CodePointIndexPair(0x%x, %d),\n", currentCodePoint, i);
      previousCodePoint = currentCodePoint;
      previousIndex = i;
      numberOfPairs++;
    }
  }
  fprintf(output, "};\n\n");

  fprintf(output, "static constexpr size_t tableLength = %d;\n\n", numberOfPairs);
}

void fillGlyphBuffer(uint8_t * uncompressedGlyphBuffer,
                     int character,
                     image_t * bitmap_image,
                     int grid_width,
                     int glyph_width,
                     int glyph_height,
                     int grid_size,
                     int grayscaleBitsPerPixel,
                     int sizeOfUncompressedGlyphBuffer) {
  int gridColumn = (character % grid_width * (glyph_width + grid_size));
  int gridRow = (character / grid_width * (glyph_height + grid_size));
  uint8_t accumulator = 0;
  int uncompressedGlyphBufferIndex = 0;
  int numberOfValuesAccumulated = 0;
  for (int y = 0; y < glyph_height; y++) {
    for (int x = 0; x < glyph_width; x++) {
      pixel_t * pixel =
          &bitmap_image->pixels[(gridRow + y) * bitmap_image->width + (x + gridColumn)];

      uint8_t grayscaleValue = (0xFF - pixel->green) >> (8 - grayscaleBitsPerPixel);
      accumulator = (accumulator << grayscaleBitsPerPixel) | grayscaleValue;
      if (numberOfValuesAccumulated++ == (8 / grayscaleBitsPerPixel) - 1) {
        uncompressedGlyphBuffer[uncompressedGlyphBufferIndex++] = accumulator;
        accumulator = 0;
        numberOfValuesAccumulated = 0;
      }
    }
  }
  ENSURE(accumulator == 0, "Discarded accumulator data (accumulator = %d)", accumulator);
  ENSURE(numberOfValuesAccumulated == 0,
         "Discarded accumulator data (numberOfValuesAccumulated = %d)",
         numberOfValuesAccumulated);
  ENSURE(uncompressedGlyphBufferIndex == sizeOfUncompressedGlyphBuffer,
         "Error filling uncompressed buffer, only %d out of %d",
         uncompressedGlyphBufferIndex,
         sizeOfUncompressedGlyphBuffer);
}

void writeGlyphData(FILE * output, image_t * bitmap_image,
                     int grid_width,
                     int glyph_width,
                     int glyph_height,
                     int grid_size) {
  fprintf(output, "static constexpr KDCoordinate glyphWidth = %d;\n\n", glyph_width);
  fprintf(output, "static constexpr KDCoordinate glyphHeight = %d;\n\n", glyph_height);

  int grayscaleBitsPerPixel = 4;

  int sizeOfUncompressedGlyphBuffer = glyph_width * glyph_height * grayscaleBitsPerPixel / 8;
  ENSURE(8*sizeOfUncompressedGlyphBuffer == glyph_width * glyph_height * grayscaleBitsPerPixel, "Error: the glyph size (%dx%d@%dbpp) cannot fit in an integral number of bytes", glyph_width, glyph_height, grayscaleBitsPerPixel);
  uint8_t * uncompressedGlyphBuffer = (uint8_t *)malloc(sizeOfUncompressedGlyphBuffer);

  uint16_t glyphDataOffset[NumberOfCodePoints+1];
  int maxGlyphDataSize = NumberOfCodePoints* sizeOfUncompressedGlyphBuffer;
  uint8_t * glyphData = (uint8_t *)malloc(maxGlyphDataSize);
  uint16_t lastOffset = 0;

  for (int character = 0; character < NumberOfCodePoints; character++) {
    fillGlyphBuffer(uncompressedGlyphBuffer,
                    character,
                    bitmap_image,
                    grid_width,
                    glyph_width,
                    glyph_height,
                    grid_size,
                    grayscaleBitsPerPixel,
                    sizeOfUncompressedGlyphBuffer);

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

  // Write the end of array so that we can extract the last size as glyphDataOffset[index + 1] - glyphDataOffset[index]
  glyphDataOffset[NumberOfCodePoints] = lastOffset;

  fprintf(output, "static constexpr uint16_t glyphDataOffset[%d] = {", NumberOfCodePoints + 1);
  prettyPrintArray(output, 80, 2, glyphDataOffset, NumberOfCodePoints + 1);
  fprintf(output, "};\n\n");

  size_t finalDataSize = lastOffset;
  size_t initialDataSize = NumberOfCodePoints * glyph_width * glyph_height;

  fprintf(output, "/* Rasterized  = %5zu bytes (%d glyphs x %d pixels)\n", initialDataSize, NumberOfCodePoints, glyph_width*glyph_height);
  fprintf(output, " * Downsampled = %5lu bytes (1/%d of rasterized)\n", initialDataSize*grayscaleBitsPerPixel/8, 8/grayscaleBitsPerPixel);
  fprintf(output, " * Compressed  = %5zu bytes (%.2f%% of rasterized) */\n", finalDataSize, 100.0*finalDataSize/initialDataSize);

  fprintf(output, "static constexpr uint8_t glyphData[%d] = {", lastOffset);
  prettyPrintArray(output, 80, 1, glyphData, lastOffset);
  fprintf(output, "};\n\n");

  free(glyphData);
  free(uncompressedGlyphBuffer);
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
