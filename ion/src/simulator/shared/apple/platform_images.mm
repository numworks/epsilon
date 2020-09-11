#include "../shared/platform.h"
#include <SDL.h>
#include <TargetConditionals.h>
#if TARGET_OS_MAC
#include <AppKit/AppKit.h>
#else
#include <UIKit/UIKit.h>
#endif

namespace Ion {
namespace Simulator {
namespace Platform {

SDL_Texture * loadImage(SDL_Renderer * renderer, const char * identifier) {
  CGImageRef cgImage = NULL;
#if TARGET_OS_MAC
  //http://lists.libsdl.org/pipermail/commits-libsdl.org/2016-December/001235.html
  [[[NSApp windows] firstObject] setColorSpace:[NSColorSpace sRGBColorSpace]];

  NSImage * nsImage = [NSImage imageNamed:[NSString stringWithUTF8String:identifier]];
  cgImage = [nsImage CGImageForProposedRect:NULL context:NULL hints:0];
#else
  cgImage = [[UIImage imageNamed:[NSString stringWithUTF8String:identifier]] CGImage];
#endif
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

}
}
}
