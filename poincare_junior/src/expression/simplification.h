#ifndef POINCARE_EXPRESSION_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_SIMPLIFICATION_H

#include <poincare_junior/src/memory/type_block.h>

namespace Poincare {

class Simplification {
public:
  static void BasicReduction(TypeBlock * block);
  static void ShallowBeautify(TypeBlock * block) {}

  static void DivisionReduction(TypeBlock * block);
  static void SubtractionReduction(TypeBlock * block);
  static TypeBlock * DistributeMultiplicationOverAddition(TypeBlock * block);
  static TypeBlock * Flatten(TypeBlock * treeBlock);
private:
  static void ProjectionReduction(TypeBlock * block, TypeBlock * (*PushProjectedEExpression)(), TypeBlock * (*PushInverse)());
};
}

#endif

