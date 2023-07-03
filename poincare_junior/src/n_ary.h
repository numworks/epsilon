#ifndef POINCARE_N_ARY_H
#define POINCARE_N_ARY_H

#include <poincare_junior/src/expression/comparison.h>
#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class NAry {
 public:
  static void AddChild(EditionReference nary, EditionReference child) {
    return AddChildAtIndex(nary, child, nary.numberOfChildren());
  }
  static void AddChildAtIndex(EditionReference nary, EditionReference child,
                              int index);
  static void AddOrMergeChildAtIndex(EditionReference nary,
                                     EditionReference child, int index);
  static EditionReference DetachChildAtIndex(EditionReference nary, int index);
  static void RemoveChildAtIndex(EditionReference nary, int index);
  static void SetNumberOfChildren(EditionReference reference,
                                  size_t numberOfChildren);
  static EditionReference Flatten(EditionReference reference);
  static EditionReference SquashIfUnary(EditionReference reference);
  static EditionReference SquashIfEmpty(EditionReference reference);
  static EditionReference Sanitize(EditionReference reference);
  static bool Flatten(EditionReference* reference);
  static bool SquashIfUnary(EditionReference* reference);
  static bool SquashIfEmpty(EditionReference* reference);
  static bool Sanitize(EditionReference* reference);
  static bool Sort(Node* nary,
                   Comparison::Order order = Comparison::Order::User);
  static bool Sort(EditionReference* reference,
                   Comparison::Order order = Comparison::Order::User);
  static void SortChildren(EditionReference reference,
                           Comparison::Order order = Comparison::Order::User);
  static void SortedInsertChild(
      EditionReference nary, EditionReference child,
      Comparison::Order order = Comparison::Order::User);

 private:
  static constexpr size_t k_maxNumberOfChildren = 255;
};

}  // namespace PoincareJ

#endif
