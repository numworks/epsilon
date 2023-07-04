#ifndef POINCARE_N_ARY_H
#define POINCARE_N_ARY_H

#include <poincare_junior/src/expression/comparison.h>

namespace PoincareJ {

class NAry {
 public:
  static void AddChild(Node* nary, Node* child) {
    return AddChildAtIndex(nary, child, nary->numberOfChildren());
  }
  static void AddChildAtIndex(Node* nary, Node* child, int index);
  static void AddOrMergeChildAtIndex(Node* nary, Node* child, int index);
  static Node* DetachChildAtIndex(Node* nary, int index);
  static void RemoveChildAtIndex(Node* nary, int index);
  static void SetNumberOfChildren(Node* nary, size_t numberOfChildren);
  static bool Flatten(Node* nary);
  static bool SquashIfUnary(Node* nary);
  static bool SquashIfEmpty(Node* nary);
  static bool Sanitize(Node* nary);
  static bool Sort(Node* nary,
                   Comparison::Order order = Comparison::Order::User);
  static void SortedInsertChild(
      Node* nary, Node* child,
      Comparison::Order order = Comparison::Order::User);

 private:
  static constexpr size_t k_maxNumberOfChildren = 255;
};

}  // namespace PoincareJ

#endif
