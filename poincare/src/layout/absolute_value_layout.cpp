#include "absolute_value_layout.h"

namespace Poincare {

ExpressionLayout * AbsoluteValueLayout::clone() const {
  AbsoluteValueLayout * layout = new AbsoluteValueLayout(const_cast<AbsoluteValueLayout *>(this)->operandLayout(), true);
  return layout;
}

}
