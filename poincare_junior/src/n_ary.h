#ifndef POINCARE_N_ARY_H
#define POINCARE_N_ARY_H

#include <poincare_junior/src/expression/comparison.h>

namespace PoincareJ {

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
    return SquashIfEmpty(nary) || SquashIfUnary(nary);
  }
  static bool Sanitize(Tree* nary);
  static bool Sort(Tree* nary,
                   Comparison::Order order = Comparison::Order::System);
  static void SortedInsertChild(
      Tree* nary, Tree* child,
      Comparison::Order order = Comparison::Order::System);
  EDITION_REF_WRAP(Flatten);
  EDITION_REF_WRAP(SquashIfPossible);
  EDITION_REF_WRAP(SquashIfEmpty);
  EDITION_REF_WRAP(SquashIfUnary);
  EDITION_REF_WRAP(Sanitize);

 private:
  static constexpr size_t k_maxNumberOfChildren = 255;
};

}  // namespace PoincareJ

#endif
