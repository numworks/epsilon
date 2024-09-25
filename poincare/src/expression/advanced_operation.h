#ifndef POINCARE_EXPRESSION_ADVANCED_OPERATION_H
#define POINCARE_EXPRESSION_ADVANCED_OPERATION_H

#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

class AdvancedOperation {
 public:
  static bool ContractImRe(Tree* e);
  static bool ExpandImRe(Tree* e);
  static bool ContractAbs(Tree* e);
  static bool ExpandAbs(Tree* e);
  static bool ContractExp(Tree* e);
  static bool ExpandExp(Tree* e);
  static bool ContractMult(Tree* e);
  static bool ExpandMult(Tree* e);
  static bool ExpandPower(Tree* e);
};

}  // namespace Poincare::Internal

#endif
