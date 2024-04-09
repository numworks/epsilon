#ifndef POINCARE_EXPRESSION_DEPENDENCY_H
#define POINCARE_EXPRESSION_DEPENDENCY_H

#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

struct Dependency {
  static bool RemoveDefinedDependencies(Tree* expr);
  static bool ShallowBubbleUpDependencies(Tree* expr);
  static bool DeepRemoveUselessDependencies(Tree* expr);
};

}  // namespace Poincare::Internal
#endif
