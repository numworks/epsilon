#pragma once

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
inline bool CanBeSorted(const Tree* e) {
  // NAry types that can always be sorted:
  bool result = e->isNAry() && e->isOfType({Type::Add, Type::Mult, Type::GCD,
                                            Type::LCM, Type::DepList});
  // If this assertion fails, add missing type in the list above or below.
  assert(result || !e->isNAry() ||
         // NAry Types that can't always be sorted:
         e->isOfType({Type::Distribution, Type::List, Type::Piecewise,
                      Type::Polynomial}));
  return result;
}
bool Sanitize(Tree* nary);
// Sort a nary that can be sorted (see CanBeSorted)
bool Sort(Tree* nary, Order::OrderType order = Order::OrderType::System);
// Sort a nary that can be sorted (see CanBeSorted) or a list
bool SortMayBeList(Tree* nary,
                   Order::OrderType order = Order::OrderType::System);
void SortedInsertChild(Tree* nary, Tree* child,
                       Order::OrderType order = Order::OrderType::System);
bool DeepSort(Tree* expression,
              Order::OrderType order = Order::OrderType::System);
bool ContainsSame(const Tree* nary, const Tree* value);
Tree* CloneSubRange(const Tree* nary, int startIndex, int endIndex);

}  // namespace NAry

}  // namespace Poincare::Internal
