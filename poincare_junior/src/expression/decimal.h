#ifndef POINCARE_EXPRESSION_DECIMAL_H
#define POINCARE_EXPRESSION_DECIMAL_H

#include <poincare_junior/src/memory/tree.h>
#include <poincare_junior/src/memory/type_block.h>

namespace PoincareJ {

class Decimal final {
 public:
  // Decimal<2>(21012)  -> 210.12
  // Decimal<-2>(21012) -> 2101200.
  static int8_t DecimalOffset(const Tree* tree) { return tree->nodeValue(0); }
  static void Project(Tree* tree);

 private:
  static inline void assertValidDecimal(const Tree* tree) {
    assert(tree->isDecimal());
  }
};

}  // namespace PoincareJ

#endif
