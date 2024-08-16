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
    AdditionBeautification,
    // Order of the extended real line, complexes/undef will raise SortFail
    RealLine
  };
  /* Compare returns:
   *  1 if e1 > e2
   * -1 if e1 < e2
   *  0 if e1 == e2 (trees are identical)
   *  Order depends on OrderType and may not match order of approximation.
   */
  static int Compare(const Tree* e1, const Tree* e2, OrderType order);
  static int CompareSystem(const Tree* e1, const Tree* e2) {
    return Compare(e1, e2, OrderType::System);
  }
  static bool ContainsSubtree(const Tree* tree, const Tree* subtree);

 private:
  // Compare two different trees. May return any int.
  static int CompareDifferent(const Tree* e1, const Tree* e2, OrderType order);
  static int CompareNumbers(const Tree* e1, const Tree* e2);
  static int CompareNames(const Tree* e1, const Tree* e2);
  static int CompareConstants(const Tree* e1, const Tree* e2);
  static int ComparePolynomial(const Tree* e1, const Tree* e2, OrderType order);
  static int CompareChildren(const Tree* e1, const Tree* e2, OrderType order,
                             bool backward = false);
  static int CompareLastChild(const Tree* e1, const Tree* e2, OrderType order);
  static int RealLineCompare(const Tree* e1, const Tree* e2);
};

}  // namespace Poincare::Internal

#endif
