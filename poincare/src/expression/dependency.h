#ifndef POINCARE_EXPRESSION_DEPENDENCY_H
#define POINCARE_EXPRESSION_DEPENDENCY_H

#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

struct Dependency {
  static const Tree* Main(const Tree* e) {
    assert(e->isDep());
    return e->child(k_mainIndex);
  }
  static const Tree* Dependencies(const Tree* e) {
    assert(e->isDep());
    assert(e->child(k_dependenciesIndex)->isDepList());
    return e->child(k_dependenciesIndex);
  }
  static Tree* Main(Tree* e) {
    assert(e->isDep());
    return e->child(k_mainIndex);
  }
  static Tree* Dependencies(Tree* e) {
    assert(e->isDep());
    assert(e->child(k_dependenciesIndex)->isDepList());
    return e->child(k_dependenciesIndex);
  }
  static bool DeepBubbleUpDependencies(Tree* e);
  static bool ShallowBubbleUpDependencies(Tree* e);
  static bool DeepRemoveUselessDependencies(Tree* e);
  static bool RemoveDependencies(Tree* e);

 private:
  constexpr static int k_mainIndex = 0;
  constexpr static int k_dependenciesIndex = 1;
};

}  // namespace Poincare::Internal
#endif
