#ifndef POINCARE_EXPRESSION_ORDER_H
#define POINCARE_EXPRESSION_ORDER_H

#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class Order {
 public:
  /* System optimizes the reduction while Beautification is for display.
   * TODO: Fine tune and take advantage of System OrderType during reduction. */
  enum class OrderType {
    System,
    Beautification,
    PreserveMatrices,
    /* TODO: AdditionBeautification mimics the order obtained by
     * Addition::shallowBeautify. Providing a custom order in Beautification
     * would be cleaner. */
    AdditionBeautification
    // TODO: add real comparison order (numerical)
  };
  /* Compare returns:
   *  1 if block0 > block1
   * -1 if block0 < block1
   *  0 if block0 == block1
   */
  static int Compare(const Tree* e0, const Tree* e1,
                     OrderType order = OrderType::System);
  static bool AreEqual(const Tree* e0, const Tree* e1);
  static bool ContainsSubtree(const Tree* tree, const Tree* subtree);

 private:
  static int CompareNumbers(const Tree* e0, const Tree* e1);
  static int CompareNames(const Tree* e0, const Tree* e1);
  static int CompareConstants(const Tree* e0, const Tree* e1);
  static int ComparePolynomial(const Tree* e0, const Tree* e1);
  static int CompareChildren(const Tree* e0, const Tree* e1,
                             bool backward = false);
  static int CompareLastChild(const Tree* e0, const Tree* e1);
};

}  // namespace Poincare::Internal

#endif
