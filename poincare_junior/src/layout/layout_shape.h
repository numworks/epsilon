#ifndef POINCARE_JUNIOR_LAYOUT_LAYOUT_SHAPE_H
#define POINCARE_JUNIOR_LAYOUT_LAYOUT_SHAPE_H

#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

enum class LayoutShape {
  Decimal,
  Integer,
  OneLetter,
  MoreLetters,
  BoundaryPunctuation,  // ( [ ∫
  Brace,
  Root,
  NthRoot,
  Fraction,
  RightOfPower,
  Default
};

LayoutShape LeftLayoutShape(const Tree* expr);
LayoutShape RightLayoutShape(const Tree* expr);

}  // namespace PoincareJ

#endif
