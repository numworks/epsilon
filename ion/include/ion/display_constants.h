#ifndef ION_DISPLAY_CONSTANTS_H
#define ION_DISPLAY_CONSTANTS_H

#include <kandinsky/rect.h>

namespace Ion {
namespace Display {

constexpr int Width = ION_DISPLAY_WIDTH;
constexpr int Height = ION_DISPLAY_HEIGHT;
constexpr KDRect Rect = KDRect(0, 0, Width, Height);

constexpr int WidthInTenthOfMillimeter = 576;
constexpr int HeightInTenthOfMillimeter = 432;

}  // namespace Display
}  // namespace Ion

#endif
