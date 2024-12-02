#ifndef POINCARE_EXPRESSION_TRIGONOMETRY_H
#define POINCARE_EXPRESSION_TRIGONOMETRY_H

#include <poincare/src/memory/tree_ref.h>

#include "projection.h"

namespace Poincare::Internal {

class Trigonometry final {
 public:
  static bool ReduceTrig(Tree* e);
  static bool ReduceTrigDiff(Tree* e);
  static bool ReduceATrig(Tree* e);
  static bool ReduceArcTangentRad(Tree* e);
  static bool ReduceArCosH(Tree* e);
  static bool ContractTrigonometric(Tree* e);
  static bool ExpandTrigonometric(Tree* e);

  static bool ReduceArgumentToPrincipal(Tree* e);
  static Type GetInverseType(Type type);

 private:
  static bool ReduceTrigSecondElement(Tree* e, bool* isOpposed);
};

}  // namespace Poincare::Internal

#endif
