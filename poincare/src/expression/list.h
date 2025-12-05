#pragma once

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
   * Intermediary results go through reduction. */
  static Tree* GetElement(const Tree* e, int k, Tree::Operation reduction);

  /* Turn the list expression into an explicit list. */
  static bool BubbleUp(Tree* e, Tree::Operation reduction = k_emptyOperation);

  static Tree* Fold(const Tree* list, TypeBlock type);
  static Tree* Mean(const Tree* list, const Tree* coefficients);
  // Variance and related functions : stdDev, sampleStdDev
  static Tree* Variance(const Tree* list, const Tree* coefficients,
                        TypeBlock type);

  static bool ShallowApplyListOperators(Tree* e);

  constexpr static Tree::Operation k_emptyOperation = [](Tree*) {
    return false;
  };
};

}  // namespace Poincare::Internal
