#ifndef POINCARE_EXPRESSION_DECIMAL_H
#define POINCARE_EXPRESSION_DECIMAL_H

#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

class Decimal final {
 public:
  static void Project(Tree* e);

  // Decimal(21012, 2)  -> 210.12
  // Decimal(21012, -2) -> 2101200.
  static int Serialize(
      const Tree* decimal, char* buffer, int bufferSize,
      Poincare::Preferences::PrintFloatMode mode,
      int numberOfSignificantDigits =
          PrintFloat::k_maxNumberOfSignificantDigitsInDecimals);

 private:
  static inline void assertValidDecimal(const Tree* e) {
    assert(e->isDecimal());
  }
};

}  // namespace Poincare::Internal

#endif
