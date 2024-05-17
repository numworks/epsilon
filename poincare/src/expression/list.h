#ifndef POINCARE_EXPRESSION_LIST_H
#define POINCARE_EXPRESSION_LIST_H

#include <poincare/src/memory/tree.h>

#include "simplification.h"

namespace Poincare::Internal {

struct List {
  static Tree* PushEmpty();
  static uint8_t Size(const Tree* list) {
    assert(list->type() == Type::List);
    return list->numberOfChildren();
  }

  /* Evaluate the list expression and return its k-th value or nullptr.
   * For instance: 2+{3,4} -> 5
   * Intermediary results are passed through reduction. */
  static Tree* GetElement(const Tree* listExpr, int k,
                          Tree::Operation reduction);

  /* Turn the list expression into an explicit list. */
  static bool BubbleUp(Tree* listExpr, Tree::Operation reduction);

  static Tree* Fold(const Tree* list, TypeBlock type);
  static Tree* Mean(const Tree* list, const Tree* coefficients);
  // Variance and related functions : stdDev, sampleStdDev
  static Tree* Variance(const Tree* list, const Tree* coefficients,
                        TypeBlock type);

  static bool ShallowApplyListOperators(Tree* expr);
};

}  // namespace Poincare::Internal

#endif
