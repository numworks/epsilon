#ifndef POINCARE_EXPRESSION_MULTIPLICATION_H
#define POINCARE_EXPRESSION_MULTIPLICATION_H

#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class Multiplication {
 public:
  static bool SimplifyMultiplication(Tree* u);
  EDITION_REF_WRAP(SimplifyMultiplication);

 private:
  // Return true if child has been merged with next sibling.
  static bool MergeMultiplicationChildWithNext(Tree* child);
  // Return true if child has been merged with one or more next siblings.
  static bool MergeMultiplicationChildrenFrom(Tree* child, int index,
                                              int* numberOfSiblings,
                                              bool* zero);
  /* Return true if child has been merged with siblings. Recursively merge next
   * siblings. */
  static bool SimplifyMultiplicationChildRec(Tree* child, int index,
                                             int* numberOfSiblings, bool* zero,
                                             bool* multiplicationChanged);
  // Simplify a sorted and sanitized multiplication.
  static bool SimplifySortedMultiplication(Tree* multiplication);
};

}  // namespace Poincare::Internal

#endif
