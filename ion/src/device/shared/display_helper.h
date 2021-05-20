#ifndef ION_DEVICE_SHARED_DISPLAY_HELPER_H
#define ION_DEVICE_SHARED_DISPLAY_HELPER_H

#include <kandinsky/font.h>

namespace Ion {
namespace Device {
namespace DisplayHelper {

void drawString(const char * text, KDCoordinate * yOffset, const KDFont * font);
bool checkDrawnString(const char * text, KDCoordinate * yOffset, const KDFont * font);

}
}
}

#endif
