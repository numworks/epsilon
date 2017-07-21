/* The image inliner converts PNG images to C++ code.
 *
 * Usage: inliner snake_case_image.png
 *
 * The inliner creates a .h and a .cpp file in the same directory as the input
 * file. The implementation file declares an Image in the ImageStore namespace,
 * and the header exposes a pointer to this variable. The Image embedds the
 * bitmap data in the RGB565 format. */

#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define ERROR_IF(cond, message) if (cond) { printf(message); return -1; };
#define MAX_FILENAME_LENGTH 255

void generateHeaderFromImage(FILE * file, const char * guardian, const char * variable);
void generateImplementationFromImage(FILE * file, const char * header, const char * variable, uint32_t width, uint32_t height, png_bytep * pixelsRowPointers);
void fileNameToSnakeCaseName(const char * fileName, char * snakeCaseName, size_t maxLength);
void snakeCaseNameToUpperSnakeName(const char * snakeCaseName, char * upperSnakeCaseName, size_t maxLength);
void camelCaseNameFromSnakeCaseNames(const char * snakeCaseName, const char * upperSnakeCaseName, char * camelCaseName, size_t maxLength);

// TODO: fix inliner to handle any png file
// TODO: truncate the app image dimensions to 55x56 pixels

int main(int argc, char * argv[]) {
  ERROR_IF(argc != 2, "Usage: inliner source.png");
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

  ERROR_IF(colorType != PNG_COLOR_TYPE_RGB_ALPHA, "Error: Inliner only handles RGBA PNG images.");
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

  FILE * header = fopen(headerPath, "w");
  generateHeaderFromImage(header, upperSnakeCaseName, camelCaseName);
  fclose(header);

  FILE * implementation = fopen(implementationPath, "w");
  generateImplementationFromImage(implementation, lowerSnakeCaseName, camelCaseName, width, height, rowPointers);
  fclose(implementation);

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

void generateImplementationFromImage(FILE * file, const char * header, const char * variable, uint32_t width, uint32_t height, png_bytep * pixelsRowPointers) {
  fprintf(file, "// This file is auto-generated by Inliner. Do not edit manually.\n");
  fprintf(file, "#include \"%s.h\"\n\n", header);
  fprintf(file, "#define P(c) KDColor::RGB24(c)\n\n");
  fprintf(file, "const KDColor pixels[] = {");
  for (int j=0; j<height; j++) {
    png_bytep pixelRow = pixelsRowPointers[j];
    for (int i=0; i<width; i++) {
      if ((i+j*width) % 6 == 0) {
        fprintf(file, "\n  ");
      } else {
        fprintf(file, " ");
      }
      png_bytep pixel = &(pixelRow[i*4]);
      double red = pixel[0]/255.0;
      double green = pixel[1]/255.0;
      double blue = pixel[2]/255.0;
      double alpha = pixel[3]/255.0;
      // Assume a white background (1.0, 1.0, 1.0) in the blending
      double blendedRed = red*alpha + 1.0*(1.0-alpha);
      double blendedGreen = green*alpha + 1.0*(1.0-alpha);
      double blendedBlue = blue*alpha + 1.0*(1.0-alpha);
      fprintf(file, "P(0x%02X%02X%02X)", (int)(blendedRed*0xFF), (int)(blendedGreen*0xFF), (int)(blendedBlue*0xFF));
      if (i!=width-1 || j!= height-1) {
        fprintf(file, ",");
      }
    }
  }
  fprintf(file, "\n};\n\n");
  fprintf(file, "constexpr Image image = Image(%d, %d, pixels);\n\n", width, height);
  fprintf(file, "const Image * ImageStore::%s = &image;\n", variable);
}
