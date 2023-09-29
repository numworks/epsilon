#ifndef POINCARE_JUNIOR_VERTICAL_OFFSET_LAYOUT_H
#define POINCARE_JUNIOR_VERTICAL_OFFSET_LAYOUT_H

#include "../memory/tree.h"

namespace PoincareJ {

class VerticalOffsetLayout {
 public:
  // TODO : Implement prefix and subscript logic
  static bool IsSuffixSuperscript(const Tree* node) { return true; }
  static const Tree* BaseLayout(const Tree* node, const Tree* root);
};

}  // namespace PoincareJ

#endif
