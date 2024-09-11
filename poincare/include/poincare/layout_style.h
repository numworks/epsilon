#ifndef POINCARE_LAYOUT_STYLE_H
#define POINCARE_LAYOUT_STYLE_H

#include <kandinsky/color.h>
#include <kandinsky/glyph.h>

namespace Poincare {

struct LayoutStyle : KDGlyph::Style {
  KDColor selectionColor;
  KDColor emptySquareColor;
  KDColor placeholderColor;
};

}  // namespace Poincare

#endif
