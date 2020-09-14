#include "../shared/platform.h"

#include <SDL.h>
#include <UIKit/UIKit.h>

SDL_Surface * IonSimulatorLoadImage(SDL_Renderer * renderer, const char * identifier) {
  CGImageRef cgImage = [[UIImage imageNamed:[NSString stringWithUTF8String:identifier]] CGImage];
  if (cgImage == NULL) {
    return NULL;
  }
  size_t width = CGImageGetWidth(cgImage);
  size_t height = CGImageGetHeight(cgImage);


  size_t bytesPerPixel = 4;
  size_t bitsPerPixel = bytesPerPixel*8;
  size_t bytesPerRow = bytesPerPixel * width;
  size_t bitsPerComponent = 8;

  void * bitmapData = malloc(height * width * bytesPerPixel);

  CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
  CGContextRef context = CGBitmapContextCreate(
    bitmapData, width, height,
    bitsPerComponent, bytesPerRow, colorSpace,
    kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big
  );

  CGContextDrawImage(context, CGRectMake(0, 0, width, height), cgImage);

  CGContextRelease(context);
  CGColorSpaceRelease(colorSpace);

  SDL_Surface * surface = SDL_CreateRGBSurfaceWithFormatFrom(
    bitmapData,
    width,
    height,
    bitsPerPixel,
    bytesPerRow,
    SDL_PIXELFORMAT_ABGR8888);

  free(bitmapData);

  return surface;
}
