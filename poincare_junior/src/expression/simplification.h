#ifndef POINCARE_EXPRESSION_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_SIMPLIFICATION_H

#include <poincare_junior/src/memory/edition_reference.h>
#include <utils/enums.h>

namespace Poincare {

class Simplification {
public:
  static void BasicReduction(EditionReference reference);
  static void ShallowBeautify(EditionReference reference) {}

  static void DivisionReduction(EditionReference reference);
  static void SubtractionReduction(EditionReference reference);
  static EditionReference DistributeMultiplicationOverAddition(EditionReference reference);

private:
  static void ProjectionReduction(EditionReference reference, EditionReference (*PushProjectedEExpression)(), EditionReference (*PushInverse)());
};
}

#endif

