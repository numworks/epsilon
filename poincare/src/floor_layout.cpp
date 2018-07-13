#include "floor_layout.h"

namespace Poincare {

ExpressionLayout * FloorLayout::clone() const {
  FloorLayout * layout = new FloorLayout(const_cast<FloorLayout *>(this)->operandLayout(), true);
  return layout;
}

}
