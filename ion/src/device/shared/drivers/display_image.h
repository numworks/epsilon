#ifndef ION_DEVICE_SHARED_DISPLAY_IMAGE_H
#define ION_DEVICE_SHARED_DISPLAY_IMAGE_H

#include <kandinsky/color.h>
#include <stdint.h>

namespace Ion {
namespace Device {
namespace DisplayImage {

void displayImage(KDColor * pixelBuffer, uint32_t width, uint32_t height, const uint8_t * compressedPixelData, uint32_t compressedPixelSize, int32_t yOffset = -1);
void logo();

}
}
}

#endif
