#include "rasterizer.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../../ion/src/external/lz4/lz4hc.h"

int main(int argc, char * argv[]) {
  checkInputs(argc, argv);

  FT_Library library;
  FT_Face face;
  char * fontFile = argv[1];
  int requestedGlyphWidth = atoi(argv[2]);
  int requestedGlyphHeight = atoi(argv[3]);
  int packedGlyphWidth = atoi(argv[4]);
  int packedGlyphHeight = atoi(argv[5]);
  char * fontName = argv[6];
  char * outputH = argv[7];
  char * outputCpp = argv[8];
  char * sharedFontPath = argv[9];
#ifdef GENERATE_PNG
  char * outputPng = argv[10];
#endif

  initTTF(&library, &face, fontFile, requestedGlyphWidth, requestedGlyphHeight);

  int maxWidth = 0;
  int maxAboveBaseline = 0;
  int maxBelowBaseline = 0;
  computeMetrics(face, &maxWidth, &maxAboveBaseline, &maxBelowBaseline);
  ENSURE(maxWidth > 0 && maxAboveBaseline > 0 && maxBelowBaseline > 0, "Computing bounds");

  int glyphWidth = maxWidth;
  int glyphHeight = maxAboveBaseline + maxBelowBaseline;
  checkGlyphDimensions(glyphWidth, packedGlyphWidth, glyphHeight, packedGlyphHeight);

  int glyphDataOffsetLength = 0, glyphDataLength = 0;
  uint16_t * glyphDataOffset = (uint16_t *)malloc((NumberOfCodePoints + 1) * sizeof(uint16_t *));
  int sizeOfUncompressedGlyphBuffer = glyphWidth * glyphHeight *  k_grayscaleBitsPerPixel / 8;
  int glyphDataBufferSize = NumberOfCodePoints * sizeOfUncompressedGlyphBuffer;
  uint8_t * glyphData = (uint8_t *)malloc(glyphDataBufferSize);

  int numberOfCodePointsPairs = writeCodePointToGlyphTableFiles(sharedFontPath, k_grayscaleBitsPerPixel,
                                                     sizeOfUncompressedGlyphBuffer);
  generateGlyphData(face, glyphDataOffset, &glyphDataOffsetLength, glyphData, glyphDataBufferSize,
                    &glyphDataLength, glyphWidth, glyphHeight, maxAboveBaseline,
                    k_grayscaleBitsPerPixel);
  writeFontHeaderFile(outputH, fontName, glyphWidth, glyphHeight);
  writeFontSourceFile(outputCpp, fontName, glyphWidth, glyphHeight, glyphDataOffset,
                      glyphDataOffsetLength, glyphData, glyphDataLength, k_grayscaleBitsPerPixel,
                      numberOfCodePointsPairs);

#if GENERATE_PNG
  storeRenderedGlyphsImage(outputPng, face, glyphWidth, glyphHeight, maxAboveBaseline);
#endif

  return 0;
}

void checkInputs(int argc, char * argv[]) {
  int expectedNumberOfArguments = 10;
#ifdef GENERATE_PNG
  expectedNumberOfArguments++;
#endif
  if (argc != expectedNumberOfArguments) {
#ifdef GENERATE_PNG
    fprintf(stderr,
            "Usage: %s fontFile requestedGlyphWidth requestedGlyphHeight packedGlyphWidth packedGlyphHeight fontName outputH outputCpp sharedFontPath outputPng\n",
            argv[0]);
#else
    fprintf(stderr, "Usage: %s fontFile requestedGlyphWidth requestedGlyphHeight packedGlyphWidth packedGlyphHeight fontName outputH outputCpp sharedFontPath\n",
            argv[0]);
#endif
    fprintf(stderr, "  fontFile: Path of the font file to load\n");
    fprintf(stderr, "  requestedGlyphWidth: Width of bitmap glyphs, in pixels\n");
    fprintf(stderr, "  requestedGlyphHeight: Height of bitmap glyphs, in pixels\n");
    fprintf(stderr, "  packedGlyphWidth: Minimal glyph width in pixels. Pass 0 if unsure.\n");
    fprintf(stderr, "  packedGlyphHeight: Minimal glyph height in pixels. Pass 0 if unsure.\n");
    fprintf(stderr, "  fontName: name of the loaded font\n");
    fprintf(stderr, "  outputH: Name of the generated C header file\n");
    fprintf(stderr, "  outputCpp: Name of the generated C source file\n");
    fprintf(stderr, "  sharedFontPath: Name of the generated shared C codepoint to glyph index source file\n");
#ifdef GENERATE_PNG
    fprintf(stderr, "  outputPng: Name of the generated PNG file\n");
#endif
    exit(1);
  }

  // Check CodePoints is sorted
  for (int i = 0; i < NumberOfCodePoints - 1; i++) {
    ENSURE(CodePoints[i] < CodePoints[i + 1], "CodePoints array must be sorted: %d > %d",
           CodePoints[i], CodePoints[i + 1]);
  }
}

void initTTF(FT_Library * library, FT_Face * face, const char * fontFile, int requestedGlyphWidth,
             int requestedGlyphHeight) {
  ENSURE(!FT_Init_FreeType(library), "Initializing library");
  // 0 means we're picking the first face in the provided file
  ENSURE(!FT_New_Face(*library, fontFile, 0, face), "Loading font file %s", fontFile);
  ENSURE(!FT_Set_Pixel_Sizes(*face, requestedGlyphWidth, requestedGlyphHeight),
         "Setting face pixel size to %dx%d", requestedGlyphWidth, requestedGlyphHeight);
}

void computeMetrics(FT_Face face, int * maxWidth, int * maxAboveBaseline, int * maxBelowBaseline) {
  /* Glyph metrics are complicated. Here are some useful links:
   * https://www.freetype.org/freetype2/docs/glyphs/metrics.png
   * https://www.freetype.org/freetype2/docs/glyphs/glyphs-3.html
   * https://www.freetype.org/freetype2/docs/reference/ft2-basic_types.html#FT_Bitmap */
  for (int i = 0; i < NumberOfCodePoints; i++) {
    wchar_t codePoint = CodePoints[i];
    loadCodePoint(face, codePoint, -1);
    int aboveBaseline = face->glyph->bitmap_top;
    int belowBaseline = face->glyph->bitmap.rows - face->glyph->bitmap_top;
    int width = face->glyph->bitmap_left + face->glyph->bitmap.width;
    *maxWidth = max(width, *maxWidth);
    *maxAboveBaseline = max(aboveBaseline, *maxAboveBaseline);
    *maxBelowBaseline = max(belowBaseline, *maxBelowBaseline);
  }
}

void checkGlyphDimensions(int glyphWidth, int requestedGlyphWidth, int glyphHeight,
                          int requestedGlyphHeight) {
  if (requestedGlyphWidth != 0) {
    ENSURE(glyphWidth == requestedGlyphWidth,
           "Expecting a packed glyph width of %d but got %d instead", requestedGlyphWidth,
           glyphWidth);
  } else {
    printf("Computed requestedGlyphWidth = %d\n", glyphWidth);
  }
  if (requestedGlyphHeight != 0) {
    ENSURE(glyphHeight == requestedGlyphHeight,
           "Expecting a packed glyph height of %d but got %d instead", requestedGlyphHeight,
           glyphHeight);
  } else {
    printf("Computed packedGlyphHeight = %d\n", glyphHeight);
  }
}

int writeCodePointToGlyphTableFiles(const char * cppFilename,
                                    int grayscaleBitsPerPixel,
                                    int maxGlyphPixelCount) {
  // source
  FILE * output = fopen(cppFilename, "w");
  fprintf(output, "/* This file is auto-generated by the rasterizer */\n\n");
  fprintf(output, "#include <kandinsky/font.h>\n\n");
  int numberOfCodePointsPairs = writeCodePointIndexPairTable(output);

  fprintf(output, "const KDFont::CodePointIndexPair * KDFont::s_CodePointToGlyphIndex = table;\n");
  fprintf(output, "const size_t KDFont::s_codePointPairsTableLength = %d;\n\n",
          numberOfCodePointsPairs);
  fclose(output);
  return numberOfCodePointsPairs;
}

int writeCodePointIndexPairTable(FILE * output) {
  int previousIndex = -1;
  uint32_t previousCodePoint = 0;
  int numberOfPairs = 0;

  fprintf(output, "constexpr static KDFont::CodePointIndexPair table[] = {\n");
  for (int i = 0; i < NumberOfCodePoints; i++) {
    int currentCodePoint = CodePoints[i];
    if (currentCodePoint != (previousCodePoint + (i - previousIndex))) {
      fprintf(output, "  KDFont::CodePointIndexPair(0x%x, %d),\n", currentCodePoint, i);
      previousCodePoint = currentCodePoint;
      previousIndex = i;
      numberOfPairs++;
    }
  }
  fprintf(output, "};\n\n");
  return numberOfPairs;
}

void writeFontHeaderFile(const char * fontHeaderFilename, const char * fontName, int glyphWidth,
                         int glyphHeight) {
  FILE * fontFile = fopen(fontHeaderFilename, "w");
  fprintf(fontFile, "#ifndef KANDINSKY_%s_H\n", fontName);
  fprintf(fontFile, "#define KANDINSKY_%s_H\n\n", fontName);

  fprintf(fontFile, "/* This file is auto-generated by the rasterizer */\n\n");

  fprintf(fontFile, "#include <kandinsky/coordinate.h>\n\n");

  fprintf(fontFile, "class %s {\n", fontName);
  fprintf(fontFile, "public:\n");
  // glyphSize
  fprintf(fontFile, "  constexpr static KDCoordinate k_glyphWidth = %d;\n", glyphWidth);
  fprintf(fontFile, "  constexpr static KDCoordinate k_glyphHeight = %d;\n", glyphHeight);
  fprintf(fontFile, "};\n\n");

  fprintf(fontFile, "#endif\n");
  fclose(fontFile);
}

void writeFontSourceFile(const char * fontSourceFilename, const char * fontName, int glyphWidth,
                         int glyphHeight, uint16_t * glyphDataOffset, int glyphDataOffsetLength,
                         uint8_t * glyphData, int glyphDataLength, int grayscaleBitsPerPixel,
                         int CodePointToGlyphIndexLength) {
  FILE * fontFile = fopen(fontSourceFilename, "w");

  fprintf(fontFile, "/* This file is auto-generated by the rasterizer */\n\n");

  fprintf(fontFile, "#include \"%s.h\"\n", fontName);
  fprintf(fontFile, "#include <kandinsky/font.h>\n\n");

  // glyphDataOffset
  fprintf(fontFile, "constexpr static KDCoordinate numberOfCodePoints = %d;\n", NumberOfCodePoints);
  fprintf(fontFile, "constexpr static uint16_t glyphDataOffset[numberOfCodePoints + 1] = {");
  prettyPrintArray(fontFile, 80, 2, glyphDataOffset, glyphDataOffsetLength);
  fprintf(fontFile, "};\n\n");

  size_t finalDataSize = glyphDataLength;
  size_t initialDataSize = NumberOfCodePoints * glyphWidth * glyphHeight;

  fprintf(fontFile, "/* Rasterized  = %5zu bytes (%d glyphs x %d pixels)\n", initialDataSize,
          NumberOfCodePoints, glyphWidth * glyphHeight);
  fprintf(fontFile, " * Downsampled = %5lu bytes (1/%d of rasterized)\n",
          initialDataSize * grayscaleBitsPerPixel / 8, 8 / grayscaleBitsPerPixel);
  fprintf(fontFile, " * Compressed  = %5zu bytes (%.2f%% of rasterized) */\n", finalDataSize,
          100.0 * finalDataSize / initialDataSize);

  // glyphData
  fprintf(fontFile, "constexpr static uint8_t glyphData[%d] = {", glyphDataLength);
  prettyPrintArray(fontFile, 80, 1, glyphData, glyphDataLength);
  fprintf(fontFile, "};\n\n");

  // Font instanciation
  fprintf(fontFile,
          "const KDFont KDFont::private%s(%s::k_glyphWidth, %s::k_glyphHeight, glyphDataOffset, glyphData);\n",
          fontName, fontName, fontName);

  fclose(fontFile);
}

void loadCodePoint(FT_Face face, wchar_t codePoint, int combinedCharactersShift) {
  // Load codePoint into face->glyph
  // FT_LOAD_RENDER: Render the glyph upon load
  ENSURE(!FT_Load_Char(face, codePoint, FT_LOAD_RENDER), "Loading character 0x%08x", codePoint);
  if (combinedCharactersShift > 0  && face->glyph->bitmap_left < 0) {
    /* Combining characters are shifted left in FreeType so as to combine with the previous
      * character We don't want that, so we move it back right */
    face->glyph->bitmap_left += combinedCharactersShift;
  }
}

void drawGlyphOnBuffer(FT_GlyphSlot glyph, int glyphWidth, int glyphHeight, int maxAboveBaseline,
                       uint8_t * glyphBuffer, int sizeOfUncompressedGlyphBuffer,
                       int grayscaleBitsPerPixel) {
  int bitmap_right = glyph->bitmap_left + glyph->bitmap.width;
  int bitmap_top = maxAboveBaseline - glyph->bitmap_top;
  int bitmap_bottom = bitmap_top + glyph->bitmap.rows;

  uint8_t accumulator = 0;
  int uncompressedGlyphBufferIndex = 0;
  int numberOfValuesAccumulated = 0;
  for (int y = 0; y < glyphHeight; y++) {
    for (int x = 0; x < glyphWidth; x++) {
      unsigned char grayscalePixel;
      if (x >= glyph->bitmap_left && x < bitmap_right && y >= bitmap_top && y < bitmap_bottom) {
        // We're in the buffer
        grayscalePixel = glyph->bitmap.buffer[(y - bitmap_top) * glyph->bitmap.pitch +
                                              (x - glyph->bitmap_left)] >>
                         (8 - grayscaleBitsPerPixel);
      } else {
        grayscalePixel = 0;
      }

      accumulator = (accumulator << grayscaleBitsPerPixel) | grayscalePixel;
      if (numberOfValuesAccumulated++ == (8 / grayscaleBitsPerPixel) - 1) {
        glyphBuffer[uncompressedGlyphBufferIndex++] = accumulator;
        accumulator = 0;
        numberOfValuesAccumulated = 0;
      }
    }
  }
  ENSURE(accumulator == 0, "Discarded accumulator data (accumulator = %d)", accumulator);
  ENSURE(numberOfValuesAccumulated == 0,
         "Discarded accumulator data (numberOfValuesAccumulated = %d)", numberOfValuesAccumulated);
  ENSURE(uncompressedGlyphBufferIndex == sizeOfUncompressedGlyphBuffer,
         "Error filling uncompressed buffer, only %d out of %d", uncompressedGlyphBufferIndex,
         sizeOfUncompressedGlyphBuffer);
}

int compressAndAppend(uint8_t * uncompressedGlyphBuffer, int sizeOfUncompressedGlyphBuffer,
                      uint8_t * glyphData, int glyphDataOffset, int maxGlyphDataSize) {
  int sizeOfCompressedGlyphBuffer = LZ4_compress_HC(
      (const char *)uncompressedGlyphBuffer, (char *)(glyphData + glyphDataOffset),
      sizeOfUncompressedGlyphBuffer, maxGlyphDataSize - glyphDataOffset, LZ4HC_CLEVEL_MAX);
  return sizeOfCompressedGlyphBuffer;
}

void generateGlyphData(FT_Face face, uint16_t * glyphDataOffset, int * glyphDataOffsetLength,
                       uint8_t * glyphData, int sizeOfGlyphData, int * glyphDataLength,
                       int glyphWidth, int glyphHeight, int maxAboveBaseline,
                       int grayscaleBitsPerPixel) {
  int sizeOfUncompressedGlyphBuffer = glyphWidth * glyphHeight * grayscaleBitsPerPixel / 8;
  ENSURE(8 * sizeOfUncompressedGlyphBuffer == glyphWidth * glyphHeight * grayscaleBitsPerPixel,
         "Error: the glyph size (%dx%d@%dbpp) cannot fit in an integral number of bytes",
         glyphWidth, glyphHeight, grayscaleBitsPerPixel);
  uint8_t * uncompressedGlyphBuffer = (uint8_t *)malloc(sizeOfUncompressedGlyphBuffer);
  int offset = 0;
  // Start with a 0
  glyphDataOffset[0] = 0;
  *glyphDataOffsetLength = 1;

  for (int index = 0; index < NumberOfCodePoints; index++) {
    wchar_t codePoint = CodePoints[index];
    loadCodePoint(face, codePoint, glyphWidth);
    // // Reinit
    // for (int i = 0; i < sizeOfUncompressedGlyphBuffer; i++) {
    //   uncompressedGlyphBuffer[i] = 0;
    // }
    drawGlyphOnBuffer(face->glyph, glyphWidth, glyphHeight, maxAboveBaseline,
                      uncompressedGlyphBuffer, sizeOfUncompressedGlyphBuffer,
                      grayscaleBitsPerPixel);
    offset += compressAndAppend(uncompressedGlyphBuffer, sizeOfUncompressedGlyphBuffer, glyphData,
                                offset, sizeOfGlyphData);
    glyphDataOffset[*glyphDataOffsetLength] = offset;
    (*glyphDataOffsetLength)++;
  }
  *glyphDataLength = offset;
}

static void prettyPrintArray(FILE * stream, int maxWidth, int typeSize, void * array,
                             int numberOfElements) {
  int lineWidth = 999;
  for (int i = 0; i < numberOfElements; i++) {
    int itemWidth = 1 + 2 * typeSize + 2 + 1;  // space + 0x + data + comma
    if (lineWidth + itemWidth > maxWidth) {
      fprintf(stream, "\n ");
      lineWidth = 1;
    }
    if (typeSize == 1) {
      fprintf(stream, " 0x%02x", ((uint8_t *)array)[i]);
    } else if (typeSize == 2) {
      fprintf(stream, " 0x%04x", ((uint16_t *)array)[i]);
    }
    if (i == numberOfElements - 1) {
      fprintf(stream, "\n");
    } else {
      fprintf(stream, ",");
    }
    lineWidth += itemWidth;
  }
}

#ifdef GENERATE_PNG

void storeRenderedGlyphsImage(const char * outputImage, FT_Face face, int glyphWidth,
                              int glyphHeight, int maxAboveBaseline) {
  int grid_size = 1;
  int gridWidth = 20;
  int grid_height = ((NumberOfCodePoints - 1) / gridWidth) + 1;

  // Initialize image
  image_t bitmapImage;
  bitmapImage.width = gridWidth * (glyphWidth + grid_size) - 1 * grid_size;
  bitmapImage.height = grid_height * (glyphHeight + grid_size) - 1 * grid_size;
  bitmapImage.pixels = (pixel_t *)malloc(sizeof(pixel_t) * bitmapImage.width * bitmapImage.height);
  ENSURE(bitmapImage.pixels != NULL, "Allocating bitmap image of size %dx%d at %ld bytes per pixel",
         bitmapImage.width, bitmapImage.height, sizeof(pixel_t));

  drawBackground(&bitmapImage, glyphWidth, glyphHeight, grid_size, maxAboveBaseline);
  drawAllGlyphsInImage(face, &bitmapImage, gridWidth, glyphWidth, glyphHeight, grid_size,
                       maxAboveBaseline);
  writeImageToPNGFile(&bitmapImage, outputImage);
}

void drawBackground(image_t * bitmap_image, int glyphWidth, int glyphHeight, int grid_size,
                    int maxAboveBaseline) {
  pixel_t grid_color = {.red = 0xFF, .green = 0, .blue = 0};
  pixel_t baseline_color = {.red = 0x82, .green = 0xFF, .blue = 0x82};
  pixel_t white = {.red = 0xFF, .green = 0xFF, .blue = 0xFF};
  for (int i = 0; i < bitmap_image->width; i++) {
    for (int j = 0; j < bitmap_image->height; j++) {
      pixel_t pixel;
      if (i % (glyphWidth + grid_size) >= glyphWidth ||
          j % (glyphHeight + grid_size) >= glyphHeight) {
        pixel = grid_color;
      } else if (j % (glyphHeight + grid_size) == maxAboveBaseline) {
        pixel = baseline_color;
      } else {
        pixel = white;
      }
      bitmap_image->pixels[j * bitmap_image->width + i] = pixel;
    }
  }
}

void drawGlyphInImage(FT_Bitmap * glyphBitmap, image_t * image, int xOffset, int yOffset) {
  // printf("Drawing glyph. Size is %dx%d, pitch is %d\n", glyphBitmap->width, glyphBitmap->rows,
  // glyphBitmap->pitch);
  ENSURE(glyphBitmap->pixel_mode == FT_PIXEL_MODE_GRAY, "Checking glyph is in FT_PIXEL_MODE_GRAY");
  for (int j = 0; j < glyphBitmap->rows; j++) {
    for (int i = 0; i < glyphBitmap->width; i++) {
      uint8_t glyphPixel = glyphBitmap->buffer[j * glyphBitmap->pitch + i];
      pixel_t * currentPixelPointer = &(
          image->pixels[(yOffset + j) * image->width + (xOffset + i)]);
      *currentPixelPointer = (pixel_t){.red = (0xFF - glyphPixel),
                                       .green = (0xFF - glyphPixel),
                                       .blue = (0xFF - glyphPixel)};  // Alpha blending
    }
  }
}

void drawAllGlyphsInImage(FT_Face face, image_t * bitmap_image, int gridWidth, int glyphWidth,
                          int glyphHeight, int grid_size, int maxAboveBaseline) {
  for (int i = 0; i < NumberOfCodePoints; i++) {
    wchar_t codePoint = CodePoints[i];
    int x = i % gridWidth;
    int y = i / gridWidth;
    loadCodePoint(face, codePoint, glyphWidth);
    // printf("Advances = %dx%d\n", face->glyph->bitmap_left, face->glyph->bitmap_top);
    drawGlyphInImage(&face->glyph->bitmap, bitmap_image,
                     x * (glyphWidth + grid_size) + face->glyph->bitmap_left,
                     y * (glyphHeight + grid_size) + maxAboveBaseline - face->glyph->bitmap_top);
  }
}

void writeImageToPNGFile(image_t * image, const char * filename) {
  FILE * file = fopen(filename, "wb");  // Write in binary mode
  ENSURE(file != NULL, "Opening file %s", filename);

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  ENSURE(png != NULL, "Allocating PNG write structure");

  png_infop info = png_create_info_struct(png);
  ENSURE(info != NULL, "Allocating info structure");

  png_init_io(png, file);

  png_set_IHDR(png, info, image->width, image->height,
               8,  // Number of bits per channel
               PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  png_write_info(png, info);

  for (int j = 0; j < image->height; j++) {
    png_write_row(png, (png_bytep)(image->pixels + j * image->width));
  }

  png_write_end(png, NULL);

  png_free_data(png, info, PNG_FREE_ALL, -1);  // -1 = all items
  png_destroy_write_struct(&png, (png_infopp)NULL);
  fclose(file);
}
#endif
