#include "dependency.h"

#include "poincare_junior/src/expression/k_tree.h"
#include "poincare_junior/src/expression/set.h"
#include "poincare_junior/src/memory/block.h"
#include "poincare_junior/src/memory/edition_reference.h"

namespace PoincareJ {

bool Dependency::ShallowBubbleUpDependencies(Tree* expr) {
  assert(expr->type() != BlockType::Dependency);
  EditionReference end = expr->nextTree();
  int numberOfSets = 0;
  for (Tree* child : expr->children()) {
    if (child->type() == BlockType::Dependency) {
      // Move dependency list at the end
      MoveTreeAtNode(end, child->childAtIndex(1));
      numberOfSets++;
      // Remove Dependency block in child
      child->removeNode();
    }
  }
  if (numberOfSets > 0) {
    while (numberOfSets > 1) {
      end = Set::Union(end, end->nextTree());
      numberOfSets--;
    }
    expr->cloneNodeAtNode(KDep);
    return true;
  }
  return false;
};

}  // namespace PoincareJ
