#ifndef POINCARE_EXPRESSION_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_SIMPLIFICATION_H

#include <poincare_junior/src/memory/edition_reference.h>
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
  static void BasicReduction(EditionReference reference);
  static void ShallowBeautify(EditionReference reference) {}

  static void DivisionReduction(EditionReference reference);
  static void SubtractionReduction(EditionReference reference);
  static EditionReference DistributeMultiplicationOverAddition(EditionReference reference);
private:
  static void ProjectionReduction(EditionReference reference, Node (*PushProjectedEExpression)(), Node (*PushInverse)());
};
}

#endif

