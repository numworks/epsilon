#include "../shared/platform.h"

#include <SDL.h>
#include <AppKit/AppKit.h>

SDL_Texture * IonSimulatorLoadImage(SDL_Renderer * renderer, const char * identifier, bool withTransparency, uint8_t alpha) {
  NSImage * nsImage = [NSImage imageNamed:[NSString stringWithUTF8String:identifier]];
  CGImageRef cgImage = [nsImage CGImageForProposedRect:NULL
                                               context:NULL
                                                 hints:0];
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

  SDL_SetColorKey(surface, withTransparency, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
  SDL_SetSurfaceAlphaMod(surface, alpha);

  SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

  free(bitmapData);
  SDL_FreeSurface(surface);

  return texture;
}
