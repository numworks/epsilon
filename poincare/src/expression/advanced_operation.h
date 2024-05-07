#ifndef POINCARE_EXPRESSION_ADVANCED_OPERATION_H
#define POINCARE_EXPRESSION_ADVANCED_OPERATION_H

#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

class AdvancedOperation {
 public:
  static bool ExpandImRe(Tree* node);
  static bool ContractAbs(Tree* node);
  static bool ExpandAbs(Tree* node);
  static bool ContractExp(Tree* node);
  static bool ExpandExp(Tree* node);
  static bool ContractMult(Tree* node);
  static bool ExpandMult(Tree* node);
  static bool ExpandPower(Tree* node);
};

}  // namespace Poincare::Internal

#endif
