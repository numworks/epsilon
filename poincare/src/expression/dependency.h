#ifndef POINCARE_EXPRESSION_DEPENDENCY_H
#define POINCARE_EXPRESSION_DEPENDENCY_H

#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

struct Dependency {
  static const Tree* Main(const Tree* e) {
    assert(e->isDependency());
    return e->child(k_mainIndex);
  }
  static const Tree* Dependencies(const Tree* e) {
    assert(e->isDependency());
    assert(e->child(k_dependenciesIndex)->isDependencies());
    return e->child(k_dependenciesIndex);
  }
  static Tree* Main(Tree* e) {
    assert(e->isDependency());
    return e->child(k_mainIndex);
  }
  static Tree* Dependencies(Tree* e) {
    assert(e->isDependency());
    assert(e->child(k_dependenciesIndex)->isDependencies());
    return e->child(k_dependenciesIndex);
  }
  static bool ShallowBubbleUpDependencies(Tree* e);
  static bool DeepRemoveUselessDependencies(Tree* e);

 private:
  constexpr static int k_mainIndex = 0;
  constexpr static int k_dependenciesIndex = 1;
};

}  // namespace Poincare::Internal
#endif
