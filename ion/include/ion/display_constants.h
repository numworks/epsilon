#ifndef ION_DISPLAY_CONSTANTS_H
#define ION_DISPLAY_CONSTANTS_H

#include <kandinsky/rect.h>

namespace Ion {
namespace Display {

constexpr int Width = 320;
constexpr int Height = 240;
constexpr KDRect Rect = KDRect(0, 0, Width, Height);

constexpr int WidthInTenthOfMillimeter = 576;
constexpr int HeightInTenthOfMillimeter = 432;

}  // namespace Display
}  // namespace Ion

#endif
