#ifndef POINCARE_MEMORY_N_ARY_H
#define POINCARE_MEMORY_N_ARY_H

#include <poincare/src/expression/order.h>

namespace Poincare::Internal {

class NAry {
 public:
  static void AddChild(Tree* nary, Tree* child) {
    AddChildAtIndex(nary, child, nary->numberOfChildren());
  }
  static void AddChildAtIndex(Tree* nary, Tree* child, int index);
  static void AddOrMergeChild(Tree* nary, Tree* child) {
    AddOrMergeChildAtIndex(nary, child, nary->numberOfChildren());
  }
  static void AddOrMergeChildAtIndex(Tree* nary, Tree* child, int index);
  static Tree* DetachChildAtIndex(Tree* nary, int index);
  static void RemoveChildAtIndex(Tree* nary, int index);
  static void SetNumberOfChildren(Tree* nary, size_t numberOfChildren);
  static bool Flatten(Tree* nary);
  static bool SquashIfUnary(Tree* nary);
  static bool SquashIfEmpty(Tree* nary);
  static bool SquashIfPossible(Tree* nary) {
    return (nary->isAdd() || nary->isMult()) &&
           (SquashIfEmpty(nary) || SquashIfUnary(nary));
  }
  static bool Sanitize(Tree* nary);
  static bool Sort(Tree* nary,
                   Order::OrderType order = Order::OrderType::System);
  static void SortedInsertChild(
      Tree* nary, Tree* child,
      Order::OrderType order = Order::OrderType::System);
  static bool ContainsSame(const Tree* nary, const Tree* value);
  static Tree* CloneSubRange(const Tree* nary, int startIndex, int endIndex);

  TREE_REF_WRAP(Flatten);
  TREE_REF_WRAP(SquashIfPossible);
  TREE_REF_WRAP(SquashIfEmpty);
  TREE_REF_WRAP(SquashIfUnary);
  TREE_REF_WRAP(Sanitize);
  TREE_REF_WRAP_1D(Sort, Order::OrderType, Order::OrderType::System);

 private:
  static constexpr size_t k_maxNumberOfChildren = 255;
};

}  // namespace Poincare::Internal

#endif
