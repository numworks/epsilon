#ifndef POINCARE_EXPRESSION_LIST_H
#define POINCARE_EXPRESSION_LIST_H

#include <poincare_junior/src/memory/tree.h>

#include "simplification.h"

namespace PoincareJ {

struct List {
  static Tree* PushEmpty();
  static uint8_t Size(const Tree* list) {
    assert(list->type() == Type::List);
    return list->numberOfChildren();
  }

  /* Replace lists by their nth element in descendants, for instance,
   * 2+{3,4}->2+3 */
  static bool ProjectToNthElement(Tree* expr, int n, Tree::Operation reduction);
  static bool BubbleUp(Tree* expr, Tree::Operation reduction);

  static Tree* Fold(const Tree* list, TypeBlock type);
  static Tree* Mean(const Tree* list, const Tree* coefficients);
  // Variance and related functions : stdDev, sampleStdDev
  static Tree* Variance(const Tree* list, const Tree* coefficients,
                        TypeBlock type);

  static bool ShallowApplyListOperators(Tree* expr);
};

}  // namespace PoincareJ

#endif
