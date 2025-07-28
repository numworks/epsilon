#ifndef ION_DISPLAY_CONSTANTS_H
#define ION_DISPLAY_CONSTANTS_H

#include <kandinsky/rect.h>

namespace Ion {
namespace Display {

constexpr int Border = ION_DISPLAY_BORDER;
constexpr int WidthWithBorder = ION_DISPLAY_WIDTH;
constexpr int HeightWithBorder = ION_DISPLAY_HEIGHT;

constexpr int Width = WidthWithBorder - 2 * Border;
constexpr int Height = HeightWithBorder - 2 * Border;
constexpr KDRect Rect = KDRect(0, 0, Width, Height);

constexpr int WidthInTenthOfMillimeter = 576;
constexpr int HeightInTenthOfMillimeter = 432;

}  // namespace Display
}  // namespace Ion

#endif
