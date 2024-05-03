#ifndef POINCARE_EXPRESSION_DEPENDENCY_H
#define POINCARE_EXPRESSION_DEPENDENCY_H

#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

struct Dependency {
  static const Tree* Main(const Tree* expr) {
    assert(expr->isDependency());
    return expr->child(k_mainIndex);
  }
  static const Tree* Dependencies(const Tree* expr) {
    assert(expr->isDependency());
    return expr->child(k_dependenciesIndex);
  }
  static Tree* Main(Tree* expr) {
    assert(expr->isDependency());
    return expr->child(k_mainIndex);
  }
  static Tree* Dependencies(Tree* expr) {
    assert(expr->isDependency());
    return expr->child(k_dependenciesIndex);
  }
  static bool ShallowBubbleUpDependencies(Tree* expr);
  static bool DeepRemoveUselessDependencies(Tree* expr);

 private:
  constexpr static int k_mainIndex = 0;
  constexpr static int k_dependenciesIndex = 1;
};

}  // namespace Poincare::Internal
#endif
