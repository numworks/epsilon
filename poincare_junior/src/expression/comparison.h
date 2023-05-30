#ifndef POINCARE_EXPRESSION_COMPARISON_H
#define POINCARE_EXPRESSION_COMPARISON_H

#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Comparison {
 public:
  /* SystematicReduce ensure that no node can slip in-between two nodes of the
   * same type, making pattern matches reliable.
   * TODO: Use Order appropriately during advanced reduction.
   * TODO: Pass the parent type or a context in Compare if necessary for an
   *       Order. */
  enum class Order { SystematicReduce, Readable };
  /* Compare returns:
   *  1 if block0 > block1
   * -1 if block0 < block1
   *  0 if block0 == block1
   */
  static int Compare(const Node node0, const Node node1,
                     Order order = Order::Readable);
  static bool AreEqual(const Node node0, const Node node1);
  static bool ContainsSubtree(const Node tree, const Node subtree);

 private:
  static int CompareNumbers(const Node node0, const Node node1);
  static int CompareNames(const Node node0, const Node node1);
  static int CompareConstants(const Node node0, const Node node1);
  static int ComparePolynomial(const Node node0, const Node node1);
  static int CompareChildren(const Node node0, const Node node1,
                             ScanDirection scanDirection);
  static int CompareLastChild(const Node node0, const Node node1);
};

}  // namespace PoincareJ

#endif
