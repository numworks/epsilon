#ifndef POINCARE_MEMORY_N_ARY_H
#define POINCARE_MEMORY_N_ARY_H

#include <poincare/src/expression/order.h>

namespace Poincare::Internal {

namespace NAry {

constexpr static size_t k_maxNumberOfChildren = 255;

void AddChildAtIndex(Tree* nary, Tree* child, int index);
inline void AddChild(Tree* nary, Tree* child) {
  AddChildAtIndex(nary, child, nary->numberOfChildren());
}
void AddOrMergeChildAtIndex(Tree* nary, Tree* child, int index);
inline void AddOrMergeChild(Tree* nary, Tree* child) {
  AddOrMergeChildAtIndex(nary, child, nary->numberOfChildren());
}
Tree* DetachChildAtIndex(Tree* nary, int index);
void RemoveChildAtIndex(Tree* nary, int index);
void SetNumberOfChildren(Tree* nary, size_t numberOfChildren);
bool Flatten(Tree* nary);
bool SquashIfUnary(Tree* nary);
bool SquashIfEmpty(Tree* nary);
inline bool SquashIfPossible(Tree* nary) {
  assert(nary->isNAry());
  return (nary->isAdd() || nary->isMult()) &&
         (SquashIfEmpty(nary) || SquashIfUnary(nary));
}
bool Sanitize(Tree* nary);
bool Sort(Tree* nary, Order::OrderType order = Order::OrderType::System);
void SortedInsertChild(Tree* nary, Tree* child,
                       Order::OrderType order = Order::OrderType::System);
bool ContainsSame(const Tree* nary, const Tree* value);
Tree* CloneSubRange(const Tree* nary, int startIndex, int endIndex);

}  // namespace NAry

}  // namespace Poincare::Internal

#endif
