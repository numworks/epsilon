#ifndef POINCARE_EXPRESSION_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_SIMPLIFICATION_H

#include <poincare_junior/src/memory/node.h>

namespace Poincare {

class Simplification {
public:
  static void BasicReduction(Node node);
  static void ShallowBeautify(Node node) {}

  static void DivisionReduction(Node node);
  static void SubtractionReduction(Node node);
  static Node DistributeMultiplicationOverAddition(Node node);
  static Node Flatten(Node node);
private:
  static void ProjectionReduction(Node node, Node (*PushProjectedEExpression)(), Node (*PushInverse)());
};
}

#endif

