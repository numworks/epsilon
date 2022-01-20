/* The image inliner converts PNG images to C++ code.
 *
 * Usage: inliner snake_case_image.png
 *
 * The inliner creates a .h and a .cpp file in the same directory as the input
 * file. The implementation file declares an Image in the ImageStore namespace,
 * and the header exposes a pointer to this variable. The Image embedded the
 * bitmap data in the RGB565 format. */

#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "../../ion/src/external/lz4/lz4hc.h"

#define ERROR_IF(cond, message) if (cond) { printf(message "\n"); return -1; };
#define MAX_FILENAME_LENGTH 255

void generateHeaderFromImage(FILE * file, const char * guardian, const char * variable);
void generateImplementationFromImage(FILE * file, const char * header, const char * variable, uint32_t width, uint32_t height, png_byte colorType, png_bytep * pixelsRowPointers);
void fileNameToSnakeCaseName(const char * fileName, char * snakeCaseName, size_t maxLength);
void snakeCaseNameToUpperSnakeName(const char * snakeCaseName, char * upperSnakeCaseName, size_t maxLength);
void camelCaseNameFromSnakeCaseNames(const char * snakeCaseName, const char * upperSnakeCaseName, char * camelCaseName, size_t maxLength);

// TODO: fix inliner to handle any png file
// TODO: truncate the app image dimensions to 55x56 pixels

int main(int argc, char * argv[]) {
  ERROR_IF(argc != 4, "Usage: inliner source.png output.h output.cpp");
  const char * inputPath = argv[1];

  FILE * inputFile = fopen(inputPath, "rb");
  ERROR_IF(inputFile == NULL, "Error: could not open input file.");

  unsigned char magic[8];
  fread(magic, 1, 8, inputFile);
  ERROR_IF(png_sig_cmp(magic, 0, 8), "Error: Input file is not a PNG file");

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  ERROR_IF(png == NULL, "Could not read png struct");

  png_infop info = png_create_info_struct(png);
  ERROR_IF(info == NULL, "Could not read info struct");

  png_init_io(png, inputFile);
  png_set_sig_bytes(png, 8);

  png_read_info(png, info);

  png_uint_32 width = png_get_image_width(png, info);
  png_uint_32 height = png_get_image_height(png, info);
  png_byte colorType = png_get_color_type(png, info);
  png_byte bitDepth = png_get_bit_depth(png, info);

  ERROR_IF(colorType != PNG_COLOR_TYPE_RGB_ALPHA && colorType != PNG_COLOR_TYPE_RGB, "Error: Inliner only handles RGB/RGBA PNG images.");
  ERROR_IF(bitDepth != 8, "Error: Inliner only handles RGBA8888 PNG images.");

  png_bytep * rowPointers = (png_bytep *)malloc(sizeof(png_bytep)*height);
  for (int i=0; i<height; i++) {
    rowPointers[i] = (png_byte *)malloc(png_get_rowbytes(png, info));
  }
  png_read_image(png, rowPointers);

  char lowerSnakeCaseName[MAX_FILENAME_LENGTH];
  char upperSnakeCaseName[MAX_FILENAME_LENGTH];
  char camelCaseName[MAX_FILENAME_LENGTH];
  const char * inputFileName = inputPath;
  // Let's keep only the last path component
  for (const char * currentChar=inputPath; *currentChar != 0; currentChar++) {
    if (*currentChar == '/') {
      inputFileName = currentChar+1;
    }
  }

  fileNameToSnakeCaseName(inputFileName, lowerSnakeCaseName, MAX_FILENAME_LENGTH);
  snakeCaseNameToUpperSnakeName(lowerSnakeCaseName, upperSnakeCaseName, MAX_FILENAME_LENGTH);
  camelCaseNameFromSnakeCaseNames(lowerSnakeCaseName, upperSnakeCaseName, camelCaseName, MAX_FILENAME_LENGTH);

  /*
  char headerPath[MAX_FILENAME_LENGTH];
  size_t pathLength = strlen(inputPath);
  strcpy(headerPath, inputPath);
  // Replace the .png extension with a .h extension
  headerPath[pathLength-3] = 'h';
  headerPath[pathLength-2] = 0;

  char implementationPath[MAX_FILENAME_LENGTH];
  strcpy(implementationPath, inputPath);
  // Replace the .png extension with a .cpp extension
  implementationPath[pathLength-3] = 'c';
  implementationPath[pathLength-2] = 'p';
  implementationPath[pathLength-1] = 'p';
  */
  char * headerPath = argv[2];
  char * implementationPath = argv[3];

  FILE * header = fopen(headerPath, "w");
  generateHeaderFromImage(header, upperSnakeCaseName, camelCaseName);
  fclose(header);

  FILE * implementation = fopen(implementationPath, "w");
  generateImplementationFromImage(implementation, lowerSnakeCaseName, camelCaseName, width, height, colorType, rowPointers);
  fclose(implementation);
  
  for (int i=0; i<height; i++) {
    free(rowPointers[i]);
  }
  free(rowPointers);
  png_destroy_read_struct(&png, &info, NULL);

  fclose(inputFile);
}

void fileNameToSnakeCaseName(const char * fileName, char * snakeCaseName, size_t maxLength) {
  for (int i=0; i<maxLength; i++) {
    snakeCaseName[i] = fileName[i];
    if (fileName[i] == '.') {
      snakeCaseName[i] = 0;
      break;
    }
  }
  snakeCaseName[maxLength-1] = 0;
}

void snakeCaseNameToUpperSnakeName(const char * snakeCaseName, char * upperSnakeCaseName, size_t maxLength) {
  for (int i=0; i<maxLength; i++) {
    upperSnakeCaseName[i] = toupper(snakeCaseName[i]);
  }
}

void camelCaseNameFromSnakeCaseNames(const char * snakeCaseName, const char * upperSnakeCaseName, char * camelCaseName, size_t maxLength) {
  int j=0;
  for (int i=0; i<maxLength; i++) {
    char nextLetter = snakeCaseName[i];
    if (nextLetter == '_') {
      continue;
    }
    if (i==0 || snakeCaseName[i-1] == '_') {
      nextLetter = upperSnakeCaseName[i];
    }
    camelCaseName[j++] = nextLetter;
  }
  camelCaseName[j] = 0;
}

void generateHeaderFromImage(FILE * file, const char * guardian, const char * variable) {
  fprintf(file, "// This file is auto-generated by Inliner. Do not edit manually.\n");
  fprintf(file, "#ifndef IMAGE_STORE_%s_H\n", guardian);
  fprintf(file, "#define IMAGE_STORE_%s_H\n\n", guardian);
  fprintf(file, "#include <escher/image.h>\n\n");
  fprintf(file, "namespace ImageStore {\n\n");
  fprintf(file, "extern const Image * %s;\n\n", variable);
  fprintf(file, "};\n\n");
  fprintf(file, "#endif\n");
}

void generateImplementationFromImage(FILE * file, const char * header, const char * variable, uint32_t width, uint32_t height, png_byte colorType, png_bytep * pixelsRowPointers) {

  int sizeOfPixelBuffer = width * height * sizeof(uint16_t);
  uint16_t * pixelBuffer = (uint16_t *)malloc(sizeOfPixelBuffer);

  for (int j=0; j<height; j++) {
    png_bytep pixelRow = pixelsRowPointers[j];
    for (int i=0; i<width; i++) {
      double alpha;
      png_bytep pixel;
      if (colorType == PNG_COLOR_TYPE_RGB) {
        pixel = &(pixelRow[i*3]);
        alpha = 1.0;
      } else {
        pixel = &(pixelRow[i*4]);
        alpha = pixel[3]/255.0;
      }
      double red = pixel[0]/255.0;
      double green = pixel[1]/255.0;
      double blue = pixel[2]/255.0;
      // Assume a white background (1.0, 1.0, 1.0) in the blending
      double blendedRed = red*alpha + 1.0*(1.0-alpha);
      double blendedGreen = green*alpha + 1.0*(1.0-alpha);
      double blendedBlue = blue*alpha + 1.0*(1.0-alpha);
      uint8_t intRed = blendedRed*0xFF;
      uint8_t intGreen = blendedGreen*0xFF;
      uint8_t intBlue = blendedBlue*0xFF;
      uint16_t rgb565value = (intRed>>3)<<11 | (intGreen>>2) << 5 | (intBlue>>3);
      pixelBuffer[j*width+i] = rgb565value;
    }
  }

  int maxSizeOfCompressedPixelBuffer = LZ4_compressBound(sizeOfPixelBuffer);
  uint8_t * compressedPixelBuffer = malloc(maxSizeOfCompressedPixelBuffer);
  int sizeOfCompressedPixelBuffer = LZ4_compress_HC(
    (const char *)pixelBuffer,
    (char *)compressedPixelBuffer,
    sizeOfPixelBuffer,
    maxSizeOfCompressedPixelBuffer,
    LZ4HC_CLEVEL_MAX
  );

  assert(sizeOfCompressedPixelBuffer != 0);

  fprintf(file, "// This file is auto-generated by Inliner. Do not edit manually.\n");
  fprintf(file, "#include \"%s.h\"\n\n", header);

  fprintf(file, "// Compressed %d pixels into %d bytes (%.2f%% compression ratio)/\n", width*height, sizeOfCompressedPixelBuffer, 100.0*sizeOfCompressedPixelBuffer/sizeOfPixelBuffer);

  fprintf(file, "const uint8_t compressedPixelData[%d] = {", sizeOfCompressedPixelBuffer);
  for (int i=0; i<sizeOfCompressedPixelBuffer; i++) {
    fprintf(file, "0x%04x, ", compressedPixelBuffer[i]);
  }


  free(compressedPixelBuffer);
  free(pixelBuffer);


  fprintf(file, "\n};\n\n");
  fprintf(file, "constexpr Image image = Image(%d, %d, compressedPixelData, %d);\n\n", width, height, sizeOfCompressedPixelBuffer);
  fprintf(file, "const Image * ImageStore::%s = &image;\n", variable);
}
