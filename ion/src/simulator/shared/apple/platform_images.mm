#include "../platform.h"
#include <SDL.h>
#include <TargetConditionals.h>
#if TARGET_OS_OSX
#include <AppKit/AppKit.h>
#else
#include <UIKit/UIKit.h>
#include <MobileCoreServices/MobileCoreServices.h>
#endif

namespace Ion {
namespace Simulator {
namespace Platform {

static CGContextRef createABGR8888Context(size_t width, size_t height) {
  size_t bytesPerPixel = 4;
  size_t bytesPerRow = bytesPerPixel * width;
  size_t bitsPerComponent = 8;

  CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
  CGContextRef context = CGBitmapContextCreate(
    nullptr, // The context will allocate and take ownership of the bitmap buffer
    width, height,
    bitsPerComponent, bytesPerRow, colorSpace,
    kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big
  );
  if (colorSpace) {
    CFRelease(colorSpace);
  }
  return context;
}

SDL_Texture * loadImage(SDL_Renderer * renderer, const char * identifier) {
  CGImageRef image = nullptr;
#if TARGET_OS_OSX
  //http://lists.libsdl.org/pipermail/commits-libsdl.org/2016-December/001235.html
  [[[NSApp windows] firstObject] setColorSpace:[NSColorSpace sRGBColorSpace]];

  NSImage * nsImage = [NSImage imageNamed:[NSString stringWithUTF8String:identifier]];
  image = [nsImage CGImageForProposedRect:NULL context:NULL hints:0];
#else
  image = [[UIImage imageNamed:[NSString stringWithUTF8String:identifier]] CGImage];
#endif
  if (image == nullptr) {
    return nullptr;
  }

  size_t width = CGImageGetWidth(image);
  size_t height = CGImageGetHeight(image);

  CGContextRef context = createABGR8888Context(width, height);
  if (context == nullptr) {
    return nullptr;
  }

  void * argb8888Pixels = CGBitmapContextGetData(context);

  CGContextDrawImage(context, CGRectMake(0, 0, width, height), image);

  SDL_Texture * texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_ABGR8888,
    SDL_TEXTUREACCESS_STATIC,
    width,
    height
  );

  size_t bytesPerPixel = 4;

  SDL_UpdateTexture(
    texture,
    NULL,
    argb8888Pixels,
    bytesPerPixel * width
  );

  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

  CGContextRelease(context);

  return texture;
}

class ABGR8888Pixel {
public:
  ABGR8888Pixel(KDColor c) :
    m_red(c.red()),
    m_green(c.green()),
    m_blue(c.blue()),
    m_alpha(255) {
      // Silent warning about unused private member
      (void)m_alpha;
  }
private:
  uint8_t m_red;
  uint8_t m_green;
  uint8_t m_blue;
  uint8_t m_alpha;
};
static_assert(sizeof(ABGR8888Pixel) == 4, "ARGB8888Pixel shall be 4 bytes long");

static CGImageRef createImageWithPixels(const KDColor * pixels, int width, int height) {
  CGContextRef context = createABGR8888Context(width, height);
  if (context == nullptr) {
    return nullptr;
  }
  ABGR8888Pixel * argb8888Pixels = static_cast<ABGR8888Pixel *>(CGBitmapContextGetData(context));
  for (int i=0; i<width*height; i++) {
    argb8888Pixels[i] = ABGR8888Pixel(pixels[i]);
  }
  CGImageRef image = CGBitmapContextCreateImage(context);
  CGContextRelease(context);
  return image;
}

void copyImageToClipboard(const KDColor * pixels, int width, int height) {
  CGImageRef cgImage = createImageWithPixels(pixels, width, height);

#if TARGET_OS_OSX
  NSImage * nsImage = [[NSImage alloc] initWithCGImage:cgImage size:NSZeroSize];
  NSPasteboard * pasteboard = [NSPasteboard generalPasteboard];
  [pasteboard clearContents];
  [pasteboard writeObjects:[NSArray arrayWithObject:nsImage]];
  [nsImage release];
#else
  // TODO: Implement on iOS
#endif

  CGImageRelease(cgImage);
}


void saveImage(const KDColor * pixels, int width, int height, const char * path) {
  CGImageRef image = createImageWithPixels(pixels, width, height);

  CFURLRef url = static_cast<CFURLRef>([NSURL fileURLWithPath:[NSString stringWithUTF8String:path]]);

  CGImageDestinationRef destination = CGImageDestinationCreateWithURL(url, kUTTypePNG, 1, NULL);
  CGImageDestinationAddImage(destination, image, nil);
  CGImageDestinationFinalize(destination);

  if (destination) {
    CFRelease(destination);
  }
  CGImageRelease(image);
}


}
}
}
