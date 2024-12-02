#ifndef POINCARE_EXPRESSION_LOGARITHM_H
#define POINCARE_EXPRESSION_LOGARITHM_H

#include <poincare/src/memory/tree_ref.h>

#include "integer.h"

namespace Poincare::Internal {

class Logarithm final {
 public:
  static bool ReduceLn(Tree* e);
  static bool ContractLn(Tree* e);
  static bool ExpandLn(Tree* e);
  // ln(12/5)->2*ln(2)+ln(3)-ln(5)
  static bool ExpandLnOnRational(Tree* e);

 private:
  // ln(12)->2*ln(2)+ln(3), return nullptr if m is prime and escapeIfPrime true.
  static Tree* ExpandLnOnInteger(IntegerHandler m, bool escapeIfPrime);
};

}  // namespace Poincare::Internal

#endif
