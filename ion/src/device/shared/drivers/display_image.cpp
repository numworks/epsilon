#include "display_image.h"
#include <drivers/display.h>
#include <ion.h>
#include "logo_image.h"

namespace Ion {
namespace Device {
namespace DisplayImage {

void displayImage(KDColor * pixelBuffer, uint32_t width, uint32_t height, const uint8_t * compressedPixelData, uint32_t compressedPixelSize, int32_t yOffset) {
  Ion::decompress(
      compressedPixelData,
      reinterpret_cast<uint8_t *>(pixelBuffer),
      compressedPixelSize,
      width * height * sizeof(KDColor)
      );
  KDCoordinate y = yOffset >= 0 ? yOffset : (Ion::Display::Height - height)/2;
  Ion::Device::Display::pushRect(KDRect((Ion::Display::Width - width)/2, y, width, height), pixelBuffer);
}

void logo() {
  Ion::Device::Display::pushRectUniform(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height), KDColorWhite);
  KDColor pixelBuffer[LogoImage::k_width * LogoImage::k_height];
  displayImage(pixelBuffer, LogoImage::k_width, LogoImage::k_height, LogoImage::compressedPixelData, LogoImage::k_compressedPixelSize);
}

}
}
}

