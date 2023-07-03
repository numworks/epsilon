#ifndef POINCARE_N_ARY_H
#define POINCARE_N_ARY_H

#include <poincare_junior/src/expression/comparison.h>
#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class NAry {
 public:
  static void AddChild(Node* nary, Node* child) {
    return AddChildAtIndex(nary, child, nary->numberOfChildren());
  }
  static void AddChildAtIndex(Node* nary, Node* child, int index);
  static void AddOrMergeChildAtIndex(EditionReference nary,
                                     EditionReference child, int index);
  static EditionReference DetachChildAtIndex(Node* nary, int index);
  static void RemoveChildAtIndex(Node* nary, int index);
  static void SetNumberOfChildren(Node* nary, size_t numberOfChildren);
  static bool Flatten(Node* nary);
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
