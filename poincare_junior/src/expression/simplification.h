#ifndef POINCARE_EXPRESSION_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_SIMPLIFICATION_H

#include <poincare_junior/src/memory/node.h>
#include <utils/enums.h>

namespace Poincare {

class Simplification {
public:
  /* CompareNumber returns:
   *  1 if block0 > block1
   * -1 if block0 < block1
   *  0 if block0 == block1
   */
  static int Compare(const Node node0, const Node node1);
private:
  static int CompareNumbers(const Node node0, const Node node1);
  static int CompareNames(const Node node0, const Node node1);
  static int CompareConstants(const Node node0, const Node node1);
  static int CompareChildren(const Node node0, const Node node1, ScanDirection scanDirection);
  static int CompareFirstChild(const Node node0, const Node node1, ScanDirection scanDirection);

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

