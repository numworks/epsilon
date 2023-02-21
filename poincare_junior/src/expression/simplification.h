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

  static EditionReference DivisionReduction(EditionReference reference);
  static EditionReference SubtractionReduction(EditionReference reference);
  static EditionReference DistributeMultiplicationOverAddition(
      EditionReference reference);

 private:
  typedef EditionReference (*NumberOperation)(const Node, const Node);
  static void ReduceNumbersInNAry(EditionReference reference,
                                  NumberOperation operation);
  static EditionReference ProjectionReduction(
      EditionReference reference,
      EditionReference (*PushProjectedEExpression)(),
      EditionReference (*PushInverse)());
};

}  // namespace PoincareJ

#endif
