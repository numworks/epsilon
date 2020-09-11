#include "../shared/platform.h"

#include <SDL.h>
#include <jpeglib.h>
#include <assert.h>

#ifndef ASSETS_ADDRESS_RANGES_DECLARATION
#error Missing assets adress range declarations
#endif

ASSETS_ADDRESS_RANGES_DECLARATION
static struct {
  const char * identifier;
  unsigned char * start;
  unsigned char * end;
} resources_addresses[] = {
  ASSETS_ADDRESS_RANGES_DEFINITION
};

SDL_Texture * IonSimulatorLoadImage(SDL_Renderer * renderer, const char * identifier) {
  struct jpeg_decompress_struct info;
  struct jpeg_error_mgr err;
  info.err = jpeg_std_error(&err);

  jpeg_create_decompress(&info);

  unsigned char * jpegStart = nullptr;
  unsigned long jpegSize = 0;

  for (size_t i = 0; i < sizeof(resources_addresses)/sizeof(resources_addresses[0]); i++) {
    if (strcmp(identifier, resources_addresses[i].identifier) == 0) {
      jpegStart = resources_addresses[i].start;
      jpegSize = resources_addresses[i].end - resources_addresses[i].start;
      break;
    }
  }
  assert(jpegStart);
  jpeg_mem_src(&info, jpegStart, jpegSize);

  if (jpeg_read_header(&info, TRUE) != 1) {
    return nullptr;
  }

  jpeg_start_decompress(&info);

  int width = info.output_width;
  int height = info.output_height;
  int bytesPerPixel = info.output_components;

  unsigned char * bitmapData = new unsigned char[height * width * bytesPerPixel];

  while (info.output_scanline < info.output_height) {
    unsigned char * buffer_array[1];
    buffer_array[0] = bitmapData + info.output_scanline * width * bytesPerPixel;
    jpeg_read_scanlines(&info, buffer_array, 1);
  }

  jpeg_finish_decompress(&info);
  jpeg_destroy_decompress(&info);

  Uint32 texturePixelFormat = SDL_PIXELFORMAT_RGB24;
  assert(bytesPerPixel == SDL_BYTESPERPIXEL(texturePixelFormat));

  SDL_Texture * texture = SDL_CreateTexture(
    renderer,
    texturePixelFormat,
    SDL_TEXTUREACCESS_STATIC,
    width,
    height
  );

  SDL_UpdateTexture(
    texture,
    NULL,
    bitmapData,
    width * bytesPerPixel
  );

  delete[] bitmapData;

  return texture;
}
