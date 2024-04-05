#ifndef POINCARE_EXPRESSION_LOGARITHM_H
#define POINCARE_EXPRESSION_LOGARITHM_H

#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/memory/tree_ref.h>

namespace PoincareJ {

class Logarithm final {
 public:
  static bool SimplifyLn(Tree* u);
  EDITION_REF_WRAP(SimplifyLn);
  static bool ContractLn(Tree* node);
  EDITION_REF_WRAP(ContractLn);
  static bool ExpandLn(Tree* node);
  EDITION_REF_WRAP(ExpandLn);
  // ln(12/5)->2*ln(2)+ln(3)-ln(5)
  static bool ExpandLnOnRational(Tree* expr);

 private:
  // ln(12)->2*ln(2)+ln(3), return nullptr if m is prime and escapeIfPrime true.
  static Tree* ExpandLnOnInteger(IntegerHandler m, bool escapeIfPrime);
};

}  // namespace PoincareJ

#endif
