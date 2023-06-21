#ifndef POINCARE_EXPRESSION_COMPARISON_H
#define POINCARE_EXPRESSION_COMPARISON_H

#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Comparison {
 public:
  /* System optimizes the reduction of expressions while User is for display.
   * TODO: Fine tune and take advantage of System Order during reduction. */
  enum class Order { System, User };
  /* Compare returns:
   *  1 if block0 > block1
   * -1 if block0 < block1
   *  0 if block0 == block1
   */
  static int Compare(const Node* node0, const Node* node1,
                     Order order = Order::User);
  static bool AreEqual(const Node* node0, const Node* node1);
  static bool ContainsSubtree(const Node* tree, const Node* subtree);

 private:
  static int CompareNumbers(const Node* node0, const Node* node1);
  static int CompareNames(const Node* node0, const Node* node1);
  static int CompareConstants(const Node* node0, const Node* node1);
  static int ComparePolynomial(const Node* node0, const Node* node1);
  static int CompareChildren(const Node* node0, const Node* node1,
                             ScanDirection scanDirection);
  static int CompareLastChild(const Node* node0, const Node* node1);
};

}  // namespace PoincareJ

#endif
