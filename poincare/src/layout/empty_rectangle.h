#ifndef POINCARE_EMPTY_RECTANGLE_H
#define POINCARE_EMPTY_RECTANGLE_H

#include <kandinsky/color.h>
#include <kandinsky/context.h>
#include <kandinsky/point.h>
#include <kandinsky/size.h>
#include <poincare/layout_style.h>

namespace Poincare::Internal {

// Static methods to draw the empty yellow rectangles in layouts

class EmptyRectangle {
 public:
  enum class Color : bool { Yellow, Gray };

  static KDSize Size(KDFont::Size font, bool withMargins = true);
  static KDCoordinate Baseline(KDFont::Size font) {
    return Size(font).height() / 2;
  }

  static void DrawEmptyRectangle(KDContext* ctx, KDPoint p, KDFont::Size font,
                                 Color rectangleColor,
                                 const LayoutStyle& style);

 private:
  constexpr static KDCoordinate k_marginWidth = 1;
  constexpr static KDCoordinate k_marginHeight = 3;
};

}  // namespace Poincare::Internal

#endif
