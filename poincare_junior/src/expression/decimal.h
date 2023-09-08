#ifndef POINCARE_EXPRESSION_DECIMAL_H
#define POINCARE_EXPRESSION_DECIMAL_H

#include <poincare_junior/src/memory/tree.h>
#include <poincare_junior/src/memory/type_block.h>

namespace PoincareJ {

class Decimal final {
 public:
  // A 2 DecimalOffset means there is two digit right of "." -> 210.12
  static uint8_t DecimalOffset(const Tree* tree) { return tree->nodeValue(0); }
  static void Project(Tree* tree);

 private:
  static inline void assertValidDecimal(const Tree* tree) {
    assert(tree->type() == BlockType::Decimal);
    assert(tree->nextNode()->type().isInteger());
  }
};

}  // namespace PoincareJ

#endif
