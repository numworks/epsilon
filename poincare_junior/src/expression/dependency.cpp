#include "dependency.h"

#include "poincare_junior/src/expression/k_tree.h"
#include "poincare_junior/src/expression/parametric.h"
#include "poincare_junior/src/expression/set.h"
#include "poincare_junior/src/expression/variables.h"
#include "poincare_junior/src/memory/block.h"
#include "poincare_junior/src/memory/edition_reference.h"

namespace PoincareJ {

bool Dependency::ShallowBubbleUpDependencies(Tree* expr) {
  if (expr->isDependency()) {
    if (!expr->child(0)->isDependency()) {
      return false;
    }
    Set::Union(expr->child(0)->child(1), expr->child(1));
    expr->removeNode();
    return true;
  }
  EditionReference end = expr->nextTree();
  int numberOfSets = 0;
  int i = 0;
  for (Tree* child : expr->children()) {
    if (child->isDependency()) {
      if (expr->isParametric() && Parametric::FunctionIndex(expr) == i &&
          Variables::HasVariable(child->child(1),
                                 Parametric::k_localVariableId)) {
        // Cannot bubble a dependency relying on local variable.
        // TODO: Would it be worth to handle each dependencies independently ?
        i++;
        continue;
      }
      // Move dependency list at the end
      MoveTreeBeforeNode(end, child->child(1));
      numberOfSets++;
      // Remove Dependency block in child
      child->removeNode();
    }
    i++;
  }
  if (numberOfSets > 0) {
    while (numberOfSets > 1) {
      end = Set::Union(end, end->nextTree());
      numberOfSets--;
    }
    expr->cloneNodeBeforeNode(KDep);
    return true;
  }
  return false;
};

}  // namespace PoincareJ
