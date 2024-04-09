#ifndef POINCARE_EXPRESSION_DECIMAL_H
#define POINCARE_EXPRESSION_DECIMAL_H

#include <poincare/preferences.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

class Decimal final {
 public:
  static int8_t DecimalOffset(const Tree* tree) { return tree->nodeValue(0); }
  static void Project(Tree* tree);

  // Decimal<2>(21012)  -> 210.12
  // Decimal<-2>(21012) -> 2101200.
  static int Serialize(const Tree* decimal, char* buffer, int bufferSize,
                       Poincare::Preferences::PrintFloatMode mode,
                       int numberOfSignificantDigits);

 private:
  static inline void assertValidDecimal(const Tree* tree) {
    assert(tree->isDecimal());
  }
};

}  // namespace Poincare::Internal

#endif
