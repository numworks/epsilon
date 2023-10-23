#ifndef POINCARE_EXPRESSION_COMPARISON_H
#define POINCARE_EXPRESSION_COMPARISON_H

#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Comparison {
 public:
  /* System optimizes the reduction of expressions while User is for display.
   * TODO: Fine tune and take advantage of System Order during reduction. */
  enum class Order { System, User, PreserveMatrices, AdditionBeautification };
  /* Compare returns:
   *  1 if block0 > block1
   * -1 if block0 < block1
   *  0 if block0 == block1
   */
  static int Compare(const Tree* node0, const Tree* node1,
                     Order order = Order::System);
  static bool AreEqual(const Tree* node0, const Tree* node1);
  static bool ContainsSubtree(const Tree* tree, const Tree* subtree);

 private:
  static int CompareNumbers(const Tree* node0, const Tree* node1);
  static int CompareNames(const Tree* node0, const Tree* node1);
  static int CompareConstants(const Tree* node0, const Tree* node1);
  static int ComparePolynomial(const Tree* node0, const Tree* node1);
  static int CompareChildren(const Tree* node0, const Tree* node1,
                             bool backward = false);
  static int CompareLastChild(const Tree* node0, const Tree* node1);
};

}  // namespace PoincareJ

#endif
