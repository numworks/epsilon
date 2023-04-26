#ifndef POINCARE_EXPRESSION_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_SIMPLIFICATION_H

#include <omgpj/enums.h>
#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Simplification {
 public:
  static EditionReference SystematicReduction(EditionReference reference);
  static EditionReference ShallowBeautify(EditionReference reference) {
    return reference;
  }

  static EditionReference ContractExp(EditionReference reference);
  static EditionReference ExpandExp(EditionReference reference);
  static EditionReference ContractTrigonometric(EditionReference reference);
  static EditionReference ExpandTrigonometric(EditionReference reference);

  static EditionReference DivisionReduction(EditionReference reference);
  static EditionReference SubtractionReduction(EditionReference reference);
  static EditionReference DistributeMultiplicationOverAddition(
      EditionReference reference);

  static EditionReference SystemProjection(EditionReference reference);

 private:
  typedef EditionReference (*NumberOperation)(const Node, const Node);
  static void ReduceNumbersInNAry(EditionReference reference,
                                  NumberOperation operation);
  static EditionReference ProjectionReduction(
      EditionReference reference, Node (*PushProjectedEExpression)(),
      Node (*PushInverse)());
};

}  // namespace PoincareJ

#endif
