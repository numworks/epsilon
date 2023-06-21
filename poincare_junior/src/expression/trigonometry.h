#ifndef POINCARE_EXPRESSION_TRIGONOMETRY_H
#define POINCARE_EXPRESSION_TRIGONOMETRY_H

#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Trigonometry final {
 public:
  static bool IsDirect(const Node* node);
  static bool IsInverse(const Node* node);
};

}  // namespace PoincareJ

#endif
