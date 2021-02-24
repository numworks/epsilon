#include "../shared/platform.h"

#include <SDL.h>
#include <UIKit/UIKit.h>

SDL_Texture * IonSimulatorLoadImage(SDL_Renderer * renderer, const char * identifier) {
  CGImageRef cgImage = [[UIImage imageNamed:[NSString stringWithUTF8String:identifier]] CGImage];
  if (cgImage == NULL) {
    return NULL;
  }
  size_t width = CGImageGetWidth(cgImage);
  size_t height = CGImageGetHeight(cgImage);


  size_t bytesPerPixel = 4;
  size_t bytesPerRow = bytesPerPixel * width;
  size_t bitsPerComponent = 8;

  size_t size = height * width * bytesPerPixel;
  void * bitmapData = malloc(size);
  memset(bitmapData, 0, size);

  CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
  CGContextRef context = CGBitmapContextCreate(
    bitmapData, width, height,
    bitsPerComponent, bytesPerRow, colorSpace,
    kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big
  );

  CGContextDrawImage(context, CGRectMake(0, 0, width, height), cgImage);

  CGContextRelease(context);
  CGColorSpaceRelease(colorSpace);

  SDL_Texture * texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_ABGR8888,
    SDL_TEXTUREACCESS_STATIC,
    width,
    height
  );

  SDL_UpdateTexture(
    texture,
    NULL,
    bitmapData,
    bytesPerPixel * width
  );

  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

  free(bitmapData);

  return texture;
}
