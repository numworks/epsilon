#ifndef POINCARE_EMPTY_RECTANGLE_H
#define POINCARE_EMPTY_RECTANGLE_H

#include <kandinsky/color.h>
#include <kandinsky/context.h>
#include <kandinsky/point.h>
#include <kandinsky/size.h>

namespace Poincare {

// Static methods to draw the empty yellow rectangles in layouts

class EmptyRectangle {
public:
  enum class Color : bool {
    Yellow,
    Gray
  };
  enum class State : bool {
    Hidden,
    Visible
  };
  static KDSize RectangleSize(KDFont::Size font, bool withMargins = true);
  static KDCoordinate RectangleBaseLine(KDFont::Size font) { return RectangleSize(font).height() / 2; }
  static void DrawEmptyRectangle(KDContext * ctx, KDPoint p, KDFont::Size font, Color rectangleColor);

private:
  constexpr static KDCoordinate k_marginWidth = 1;
  constexpr static KDCoordinate k_marginHeight = 3;
};

}

#endif
