#ifndef ION_DEVICE_USERLAND_DRIVERS_DISPLAY_H
#define ION_DEVICE_USERLAND_DRIVERS_DISPLAY_H

#include <kandinsky/color.h>
#include <kandinsky/font.h>
#include <kandinsky/glyph.h>
#include <kandinsky/point.h>

namespace Ion {
namespace Display {

void drawString(const char* text, KDPoint point, KDGlyph::Style style);

}
}  // namespace Ion

#endif
