#include "ceiling_layout.h"

namespace Poincare {

ExpressionLayout * CeilingLayout::clone() const {
  CeilingLayout * layout = new CeilingLayout(const_cast<CeilingLayout *>(this)->operandLayout(), true);
  return layout;
}

}
