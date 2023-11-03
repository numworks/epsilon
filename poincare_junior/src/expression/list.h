#ifndef POINCARE_EXPRESSION_LIST_H
#define POINCARE_EXPRESSION_LIST_H

#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

struct List {
  static uint8_t Size(const Tree* list) {
    assert(list->type() == BlockType::SystemList);
    return list->numberOfChildren();
  }

  /* Replace lists by their nth element in descendants, for instance,
   * 2+{3,4}->2+3 */
  static bool ProjectToNthElement(Tree* expr, int n);

  static Tree* Fold(const Tree* list, BlockType type);
  static Tree* Mean(const Tree* list);

  static bool ShallowApplyListOperators(Tree* expr);
};

}  // namespace PoincareJ

#endif
