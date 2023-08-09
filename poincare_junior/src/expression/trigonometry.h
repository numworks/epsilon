#ifndef POINCARE_EXPRESSION_TRIGONOMETRY_H
#define POINCARE_EXPRESSION_TRIGONOMETRY_H

#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Trigonometry final {
 public:
  static bool IsDirect(const Tree* node);
  static bool IsInverse(const Tree* node);
  // Given n, return the exact expression of sin(n*π/12).
  static const Tree* ExactFormula(uint8_t n, bool isSin, bool* isOpposed);
};

}  // namespace PoincareJ

#endif
