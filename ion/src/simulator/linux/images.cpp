#include "../shared/platform.h"

#include <SDL.h>
#include <jpeglib.h>
#include <png.h>
#include <assert.h>

#include <ion/src/simulator/linux/images.h>

enum class AssetFormat {
  JPG,
  PNG
};

png_size_t readSize = 0;

void ReadDataFromMemory(png_structp png, png_bytep outBytes, png_size_t byteSize) {
   png_voidp io = png_get_io_ptr(png);
   memcpy((char *)outBytes, (char *)io + readSize, byteSize);
   readSize += byteSize;
}

bool readPNG(unsigned char * start, size_t size, unsigned char ** bitmapData, unsigned int * width, unsigned int * height, unsigned int * bytesPerPixel, Uint32 * pixelFormat) {
  readSize = 0;
  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png) {
    // Memory failure
    return false;
  }
  png_infop info = png_create_info_struct(png);
  if (!info) {
    // Memory failure
    png_destroy_read_struct(&png, NULL, NULL);
    return false;
  }
  static constexpr size_t k_pngSignatureLength = 8;
  if (png_sig_cmp((png_const_bytep)start, 0, k_pngSignatureLength) != 0) {
    // Corrupted PNG
    return false;
  }

  png_set_read_fn(png, start, ReadDataFromMemory);

  png_read_info(png, info);

  int bitDepth = 0;
  int colorType = -1;
  png_get_IHDR(png, info, width, height, &bitDepth, &colorType, nullptr, nullptr, nullptr);
  *bytesPerPixel = bitDepth*4/8;

  *pixelFormat = SDL_PIXELFORMAT_ARGB8888;
  assert(colorType == PNG_COLOR_TYPE_RGB_ALPHA);

  const png_uint_32 bytesPerRow = png_get_rowbytes(png, info);
  *bitmapData = new unsigned char[*height * bytesPerRow];
  unsigned char * rowData = *bitmapData;
  for (unsigned int rowIndex = 0; rowIndex < *height; rowIndex++) {
    png_read_row(png, static_cast<png_bytep>(rowData), nullptr);
    rowData += bytesPerRow;
  }

  png_destroy_read_struct(&png, &info, nullptr);
  return true;
}

bool readJPG(const unsigned char * start, size_t size, unsigned char ** bitmapData, unsigned int * width, unsigned int * height, unsigned int * bytesPerPixel, Uint32 * pixelFormat) {
  *pixelFormat = SDL_PIXELFORMAT_RGB24;
  struct jpeg_decompress_struct info;
  struct jpeg_error_mgr err;
  info.err = jpeg_std_error(&err);

  jpeg_create_decompress(&info);

  jpeg_mem_src(&info, start, size);

  if (jpeg_read_header(&info, TRUE) != 1) {
    return false;
  }

  jpeg_start_decompress(&info);

  *width = info.output_width;
  *height = info.output_height;
  *bytesPerPixel = info.output_components;
  *bitmapData = new unsigned char[*height * *width * *bytesPerPixel];

  while (info.output_scanline < info.output_height) {
    unsigned char * buffer_array[1];
    buffer_array[0] = *bitmapData + info.output_scanline * *width * *bytesPerPixel;
    jpeg_read_scanlines(&info, buffer_array, 1);
  }

  jpeg_finish_decompress(&info);
  jpeg_destroy_decompress(&info);
  return true;
}

SDL_Texture * IonSimulatorLoadImage(SDL_Renderer * renderer, const char * identifier) {
  static constexpr const char * jpgExtension = ".jpg";
  static constexpr const char * pngExtension = ".png";

  unsigned char * assetStart = nullptr;
  unsigned long assertSize = 0;
  AssetFormat format;

  // Find the asset corresponding to identifier
  for (size_t i = 0; i < sizeof(resources_addresses)/sizeof(resources_addresses[0]); i++) {
    if (strcmp(identifier, resources_addresses[i].identifier()) == 0) {
      if (strcmp(jpgExtension, identifier + strlen(identifier) - strlen(jpgExtension)) == 0) {
        format = AssetFormat::JPG;
      } else {
        assert(strcmp(pngExtension, identifier + strlen(identifier) - strlen(pngExtension)) == 0);
        format = AssetFormat::PNG;
      }
      assetStart = resources_addresses[i].start();
      assertSize = resources_addresses[i].end() - resources_addresses[i].start();
      break;
    }
  }
  assert(assetStart);

  unsigned int width;
  unsigned int height;
  unsigned int bytesPerPixel;
  unsigned char * bitmapData;
  Uint32 pixelFormat;

  switch (format) {
    case AssetFormat::JPG:
      if (!readJPG(assetStart, assertSize, &bitmapData, &width, &height, &bytesPerPixel, &pixelFormat)) {
        return nullptr;
      }
      break;
    default:
      assert(format == AssetFormat::PNG);
      if (!readPNG(assetStart, assertSize, &bitmapData, &width, &height, &bytesPerPixel, &pixelFormat)) {
        return nullptr;
      }
  }

  assert(bytesPerPixel == SDL_BYTESPERPIXEL(pixelFormat));

  SDL_Texture * texture = SDL_CreateTexture(
    renderer,
    pixelFormat,
    SDL_TEXTUREACCESS_STATIC,
    width,
    height
  );

  SDL_UpdateTexture(
    texture,
    nullptr,
    bitmapData,
    width * bytesPerPixel
  );

  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

  delete[] bitmapData;

  return texture;
}
