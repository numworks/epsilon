#ifndef POINCARE_N_ARY_H
#define POINCARE_N_ARY_H

#include <poincare_junior/src/expression/comparison.h>

namespace PoincareJ {

class NAry {
 public:
  static void AddChild(Tree* nary, Tree* child) {
    return AddChildAtIndex(nary, child, nary->numberOfChildren());
  }
  static void AddChildAtIndex(Tree* nary, Tree* child, int index);
  static void AddOrMergeChildAtIndex(Tree* nary, Tree* child, int index);
  static Tree* DetachChildAtIndex(Tree* nary, int index);
  static void RemoveChildAtIndex(Tree* nary, int index);
  static void SetNumberOfChildren(Tree* nary, size_t numberOfChildren);
  static bool Flatten(Tree* nary);
  static bool SquashIfUnary(Tree* nary);
  static bool SquashIfEmpty(Tree* nary);
  static bool Sanitize(Tree* nary);
  static bool Sort(Tree* nary,
                   Comparison::Order order = Comparison::Order::User);
  static void SortedInsertChild(
      Tree* nary, Tree* child,
      Comparison::Order order = Comparison::Order::User);

 private:
  static constexpr size_t k_maxNumberOfChildren = 255;
};

}  // namespace PoincareJ

#endif
