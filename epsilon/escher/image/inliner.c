/* The image inliner converts PNG images to C++ code.
 *
 * Usage:
 *   inliner snake_case_image.png output.h output.cpp [--transparent]
 *   inliner --themed light.png dark.png output.h output.cpp [--transparent]
 *   inliner --themed --selectable light.png dark.png selected_dark.png
 *       output.h output.cpp [--transparent]
 *
 * In the regular mode, the inliner creates a .h and a .cpp file declaring an
 * Image in the ImageStore namespace. The header exposes a pointer to this
 * variable. The Image embeds the bitmap data in the RGB565 format.
 *
 * In the --themed mode, the inliner reads two PNGs (light and dark) and emits
 * a single .h/.cpp pair declaring a ThemedImage in the ImageStore namespace.
 * With the additional --selectable flag, a third PNG (the selected variant of
 * the dark theme) is read and embedded as the ThemedImage's selectedDark
 * image. Without --selectable, the ThemedImage's selectedDark pointer stays
 * at nullptr. Both PNGs must have matching dimensions in practice (not
 * enforced here). */

#include <assert.h>
#include <ctype.h>
#include <png.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../../shared/omg/src/external/lz4/lz4hc.h"

#define ERROR_IF(cond, message) \
  if (cond) {                   \
    printf(message "\n");       \
    return -1;                  \
  };
#define MAX_FILENAME_LENGTH 255

typedef uint8_t bool;
#define TRUE 1
#define FALSE 0

typedef struct {
  uint32_t width;
  uint32_t height;
  uint8_t* compressedPixels;
  int compressedSize;
  int rawSize;
} CompressedImage;

int readAndCompressPng(const char* path, bool transparent,
                       CompressedImage* out);
void writeCompressedPixelArray(FILE* file, const char* arrayName,
                               const CompressedImage* image);
void writeImageConstexpr(FILE* file, const char* imageVar,
                         const char* pixelDataVar, const CompressedImage* image,
                         bool transparent);

void generateHeaderFromImage(FILE* file, const char* guardian,
                             const char* variable);
void generateImplementationFromImage(FILE* file, const char* header,
                                     const char* variable,
                                     const CompressedImage* image,
                                     bool transparent);
void generateHeaderFromThemedImage(FILE* file, const char* guardian,
                                   const char* variable);
void generateImplementationFromThemedImage(FILE* file, const char* header,
                                           const char* variable,
                                           const CompressedImage* light,
                                           const CompressedImage* dark,
                                           const CompressedImage* selectedDark,
                                           bool transparent);

void fileNameToSnakeCaseName(const char* fileName, char* snakeCaseName,
                             size_t maxLength);
void snakeCaseNameToUpperSnakeName(const char* snakeCaseName,
                                   char* upperSnakeCaseName, size_t maxLength);
void camelCaseNameFromSnakeCaseNames(const char* snakeCaseName,
                                     const char* upperSnakeCaseName,
                                     char* camelCaseName, size_t maxLength);
void deriveNamesFromPath(const char* path, char* lowerSnakeCaseName,
                         char* upperSnakeCaseName, char* camelCaseName);

// TODO: fix inliner to handle any png file
// TODO: truncate the app image dimensions to 55x56 pixels

int main(int argc, char* argv[]) {
  bool themed = FALSE;
  bool selectable = FALSE;
  bool transparent = FALSE;

  // Detect --themed flag (must be first if present).
  int argi = 1;
  if (argi < argc && strcmp(argv[argi], "--themed") == 0) {
    themed = TRUE;
    argi++;
  }

  // Detect --selectable flag (only valid with --themed, must follow it).
  if (themed && argi < argc && strcmp(argv[argi], "--selectable") == 0) {
    selectable = TRUE;
    argi++;
  }

  // Detect trailing --transparent flag.
  if (argc > 1 && strcmp(argv[argc - 1], "--transparent") == 0) {
    transparent = TRUE;
    argc--;
  }

  if (themed) {
    int expectedArgs = selectable ? 5 : 4;
    ERROR_IF(argc - argi != expectedArgs,
             "Usage: inliner --themed [--selectable] light.png dark.png "
             "[selected_dark.png] output.h output.cpp [--transparent]");
    const char* lightPath = argv[argi];
    const char* darkPath = argv[argi + 1];
    const char* selectedDarkPath = selectable ? argv[argi + 2] : NULL;
    int pathOffset = selectable ? 3 : 2;
    const char* headerPath = argv[argi + pathOffset];
    const char* implementationPath = argv[argi + pathOffset + 1];

    CompressedImage light = {0};
    CompressedImage dark = {0};
    CompressedImage selectedDark = {0};
    ERROR_IF(readAndCompressPng(lightPath, transparent, &light) != 0,
             "Error: could not read light PNG");
    ERROR_IF(readAndCompressPng(darkPath, transparent, &dark) != 0,
             "Error: could not read dark PNG");
    if (selectable) {
      ERROR_IF(
          readAndCompressPng(selectedDarkPath, transparent, &selectedDark) != 0,
          "Error: could not read selected dark PNG");
    }

    // Derive the symbol names from the output header path.
    char lowerSnakeCaseName[MAX_FILENAME_LENGTH];
    char upperSnakeCaseName[MAX_FILENAME_LENGTH];
    char camelCaseName[MAX_FILENAME_LENGTH];
    deriveNamesFromPath(headerPath, lowerSnakeCaseName, upperSnakeCaseName,
                        camelCaseName);

    FILE* header = fopen(headerPath, "w");
    generateHeaderFromThemedImage(header, upperSnakeCaseName, camelCaseName);
    fclose(header);

    FILE* implementation = fopen(implementationPath, "w");
    generateImplementationFromThemedImage(
        implementation, lowerSnakeCaseName, camelCaseName, &light, &dark,
        selectable ? &selectedDark : NULL, transparent);
    fclose(implementation);

    free(light.compressedPixels);
    free(dark.compressedPixels);
    if (selectable) {
      free(selectedDark.compressedPixels);
    }
    return 0;
  }

  ERROR_IF(argc - argi != 3,
           "Usage: inliner source.png output.h output.cpp [--transparent]");
  const char* inputPath = argv[argi];
  const char* headerPath = argv[argi + 1];
  const char* implementationPath = argv[argi + 2];

  CompressedImage image = {0};
  ERROR_IF(readAndCompressPng(inputPath, transparent, &image) != 0,
           "Error: could not read PNG");

  char lowerSnakeCaseName[MAX_FILENAME_LENGTH];
  char upperSnakeCaseName[MAX_FILENAME_LENGTH];
  char camelCaseName[MAX_FILENAME_LENGTH];
  deriveNamesFromPath(inputPath, lowerSnakeCaseName, upperSnakeCaseName,
                      camelCaseName);

  FILE* header = fopen(headerPath, "w");
  generateHeaderFromImage(header, upperSnakeCaseName, camelCaseName);
  fclose(header);

  FILE* implementation = fopen(implementationPath, "w");
  generateImplementationFromImage(implementation, lowerSnakeCaseName,
                                  camelCaseName, &image, transparent);
  fclose(implementation);

  free(image.compressedPixels);
  return 0;
}

int readAndCompressPng(const char* path, bool transparent,
                       CompressedImage* out) {
  FILE* inputFile = fopen(path, "rb");
  if (inputFile == NULL) {
    printf("Error: could not open input file %s.\n", path);
    return -1;
  }

  unsigned char magic[8];
  fread(magic, 1, 8, inputFile);
  if (png_sig_cmp(magic, 0, 8)) {
    printf("Error: %s is not a PNG file\n", path);
    fclose(inputFile);
    return -1;
  }

  png_structp png =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png == NULL) {
    fclose(inputFile);
    return -1;
  }
  png_infop info = png_create_info_struct(png);
  if (info == NULL) {
    fclose(inputFile);
    return -1;
  }

  png_init_io(png, inputFile);
  png_set_sig_bytes(png, 8);
  png_read_info(png, info);

  png_uint_32 width = png_get_image_width(png, info);
  png_uint_32 height = png_get_image_height(png, info);
  png_byte colorType = png_get_color_type(png, info);
  png_byte bitDepth = png_get_bit_depth(png, info);

  if (colorType != PNG_COLOR_TYPE_RGB_ALPHA || bitDepth != 8) {
    printf("Error: Inliner only handles RGBA8888 PNG images (%s).\n", path);
    fclose(inputFile);
    return -1;
  }

  png_bytep* rowPointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
  for (int i = 0; i < height; i++) {
    rowPointers[i] = (png_byte*)malloc(png_get_rowbytes(png, info));
  }
  png_read_image(png, rowPointers);

  int sizeOfPixelBuffer = width * height * sizeof(uint16_t);
  if (transparent) {
    sizeOfPixelBuffer += width * height * sizeof(uint8_t);
  }
  uint16_t* pixelBuffer = (uint16_t*)malloc(sizeOfPixelBuffer);

  for (int j = 0; j < height; j++) {
    png_bytep pixelRow = rowPointers[j];
    for (int i = 0; i < width; i++) {
      png_bytep pixel = &(pixelRow[i * 4]);
      double red = pixel[0] / 255.0;
      double green = pixel[1] / 255.0;
      double blue = pixel[2] / 255.0;
      double alpha = pixel[3] / 255.0;
      // Assume a white background (1.0, 1.0, 1.0) in the blending
      double blendedRed = red * alpha + 1.0 * (1.0 - alpha);
      double blendedGreen = green * alpha + 1.0 * (1.0 - alpha);
      double blendedBlue = blue * alpha + 1.0 * (1.0 - alpha);
      uint8_t intRed = blendedRed * 0xFF;
      uint8_t intGreen = blendedGreen * 0xFF;
      uint8_t intBlue = blendedBlue * 0xFF;
      uint16_t rgb565value =
          (intRed >> 3) << 11 | (intGreen >> 2) << 5 | (intBlue >> 3);
      pixelBuffer[j * width + i] = rgb565value;

      if (transparent) {
        uint8_t* transparentBuffer = (uint8_t*)(&pixelBuffer[width * height]);
        transparentBuffer[j * width + i] = (uint8_t)(alpha * 0xFF);
      }
    }
  }

  int maxSizeOfCompressedPixelBuffer = LZ4_compressBound(sizeOfPixelBuffer);
  uint8_t* compressedPixelBuffer = malloc(maxSizeOfCompressedPixelBuffer);
  int sizeOfCompressedPixelBuffer = LZ4_compress_HC(
      (const char*)pixelBuffer, (char*)compressedPixelBuffer, sizeOfPixelBuffer,
      maxSizeOfCompressedPixelBuffer, LZ4HC_CLEVEL_MAX);
  assert(sizeOfCompressedPixelBuffer != 0);

  free(pixelBuffer);
  for (int i = 0; i < height; i++) {
    free(rowPointers[i]);
  }
  free(rowPointers);
  png_destroy_read_struct(&png, &info, NULL);
  fclose(inputFile);

  out->width = width;
  out->height = height;
  out->compressedPixels = compressedPixelBuffer;
  out->compressedSize = sizeOfCompressedPixelBuffer;
  out->rawSize = sizeOfPixelBuffer;
  return 0;
}

void writeCompressedPixelArray(FILE* file, const char* arrayName,
                               const CompressedImage* image) {
  fprintf(file,
          "// Compressed %d pixels into %d bytes (%.2f%% compression ratio)\n",
          image->width * image->height, image->compressedSize,
          100.0 * image->compressedSize / image->rawSize);
  fprintf(file, "const uint8_t %s[%d] = {", arrayName, image->compressedSize);
  for (int i = 0; i < image->compressedSize; i++) {
    fprintf(file, "0x%02x, ", image->compressedPixels[i]);
  }
  fprintf(file, "\n};\n\n");
}

void writeImageConstexpr(FILE* file, const char* imageVar,
                         const char* pixelDataVar, const CompressedImage* image,
                         bool transparent) {
  fprintf(file,
          "constexpr Escher::Image %s = Escher::Image(%d, %d, %s, %d, %s);\n",
          imageVar, image->width, image->height, pixelDataVar,
          image->compressedSize, transparent ? "true" : "false");
}

void generateHeaderFromImage(FILE* file, const char* guardian,
                             const char* variable) {
  fprintf(file,
          "// This file is auto-generated by Inliner. Do not edit manually.\n");
  fprintf(file, "#ifndef IMAGE_STORE_%s_H\n", guardian);
  fprintf(file, "#define IMAGE_STORE_%s_H\n\n", guardian);
  fprintf(file, "#include <escher/image.h>\n\n");
  fprintf(file, "namespace ImageStore {\n\n");
  fprintf(file, "extern const Escher::Image * const %s;\n\n", variable);
  fprintf(file, "};\n\n");
  fprintf(file, "#endif\n");
}

void generateImplementationFromImage(FILE* file, const char* header,
                                     const char* variable,
                                     const CompressedImage* image,
                                     bool transparent) {
  fprintf(file,
          "// This file is auto-generated by Inliner. Do not edit manually.\n");
  fprintf(file, "#include \"%s.h\"\n\n", header);
  writeCompressedPixelArray(file, "compressedPixelData", image);
  writeImageConstexpr(file, "image", "compressedPixelData", image, transparent);
  fprintf(file, "\nconst Escher::Image * const ImageStore::%s = &image;\n",
          variable);
}

void generateHeaderFromThemedImage(FILE* file, const char* guardian,
                                   const char* variable) {
  fprintf(file,
          "// This file is auto-generated by Inliner. Do not edit manually.\n");
  fprintf(file, "#ifndef IMAGE_STORE_%s_H\n", guardian);
  fprintf(file, "#define IMAGE_STORE_%s_H\n\n", guardian);
  fprintf(file, "#include <escher/themed_image.h>\n\n");
  fprintf(file, "namespace ImageStore {\n\n");
  fprintf(file, "extern const Escher::ThemedImage * const %s;\n\n", variable);
  fprintf(file, "};\n\n");
  fprintf(file, "#endif\n");
}

void generateImplementationFromThemedImage(FILE* file, const char* header,
                                           const char* variable,
                                           const CompressedImage* light,
                                           const CompressedImage* dark,
                                           const CompressedImage* selectedDark,
                                           bool transparent) {
  fprintf(file,
          "// This file is auto-generated by Inliner. Do not edit manually.\n");
  fprintf(file, "#include \"%s.h\"\n\n", header);
  writeCompressedPixelArray(file, "compressedPixelDataLight", light);
  writeCompressedPixelArray(file, "compressedPixelDataDark", dark);
  writeImageConstexpr(file, "imageLight", "compressedPixelDataLight", light,
                      transparent);
  writeImageConstexpr(file, "imageDark", "compressedPixelDataDark", dark,
                      transparent);
  if (selectedDark != NULL) {
    writeCompressedPixelArray(file, "compressedPixelDataselectedDark",
                              selectedDark);
    writeImageConstexpr(file, "imageselectedDark",
                        "compressedPixelDataselectedDark", selectedDark,
                        transparent);
    fprintf(file,
            "\nconstexpr Escher::ThemedImage themedImage = "
            "Escher::ThemedImage(&imageLight, &imageDark, "
            "&imageselectedDark);\n\n");
  } else {
    fprintf(file,
            "\nconstexpr Escher::ThemedImage themedImage = "
            "Escher::ThemedImage(&imageLight, &imageDark);\n\n");
  }
  fprintf(file,
          "const Escher::ThemedImage * const ImageStore::%s = &themedImage;\n",
          variable);
}

void deriveNamesFromPath(const char* path, char* lowerSnakeCaseName,
                         char* upperSnakeCaseName, char* camelCaseName) {
  const char* fileName = path;
  for (const char* currentChar = path; *currentChar != 0; currentChar++) {
    if (*currentChar == '/') {
      fileName = currentChar + 1;
    }
  }
  fileNameToSnakeCaseName(fileName, lowerSnakeCaseName, MAX_FILENAME_LENGTH);
  snakeCaseNameToUpperSnakeName(lowerSnakeCaseName, upperSnakeCaseName,
                                MAX_FILENAME_LENGTH);
  camelCaseNameFromSnakeCaseNames(lowerSnakeCaseName, upperSnakeCaseName,
                                  camelCaseName, MAX_FILENAME_LENGTH);
}

void fileNameToSnakeCaseName(const char* fileName, char* snakeCaseName,
                             size_t maxLength) {
  for (int i = 0; i < maxLength; i++) {
    snakeCaseName[i] = fileName[i];
    if (fileName[i] == '.') {
      snakeCaseName[i] = 0;
      break;
    }
  }
  snakeCaseName[maxLength - 1] = 0;
}

void snakeCaseNameToUpperSnakeName(const char* snakeCaseName,
                                   char* upperSnakeCaseName, size_t maxLength) {
  for (int i = 0; i < maxLength; i++) {
    char nextLetter = snakeCaseName[i];
    upperSnakeCaseName[i] = toupper(nextLetter);
    if (nextLetter == 0) {
      break;
    }
  }
}

void camelCaseNameFromSnakeCaseNames(const char* snakeCaseName,
                                     const char* upperSnakeCaseName,
                                     char* camelCaseName, size_t maxLength) {
  int j = 0;
  for (int i = 0; i < maxLength; i++) {
    char nextLetter = snakeCaseName[i];
    if (nextLetter == '_') {
      continue;
    }
    if (i == 0 || snakeCaseName[i - 1] == '_') {
      nextLetter = upperSnakeCaseName[i];
    }
    if (nextLetter == 0) {
      break;
    }
    camelCaseName[j++] = nextLetter;
  }
  camelCaseName[j] = 0;
}
