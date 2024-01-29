#include "dependency.h"

#include "poincare_junior/src/expression/k_tree.h"
#include "poincare_junior/src/expression/parametric.h"
#include "poincare_junior/src/expression/set.h"
#include "poincare_junior/src/expression/variables.h"
#include "poincare_junior/src/memory/block.h"
#include "poincare_junior/src/memory/edition_reference.h"
#include "poincare_junior/src/n_ary.h"

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
  for (Tree* exprChild : expr->children()) {
    if (exprChild->isDependency()) {
      if (expr->isParametric() && Parametric::FunctionIndex(expr) == i) {
        /* diff(dep(x, {ln(x), z}), x, y) ->
         * dep(diff(x, x, y), {diff(ln(x), x, y), z})
         * TODO: Keeping the dependency in the parametric would be more optimal,
         * but we would have to handle them along the simplification process
         * (especially difficult in the advanced and systematic reduction). */
        Tree* exprChildDep = exprChild->child(1);
        int numberOfDependencies = exprChildDep->numberOfChildren();
        for (int j = 0; j < numberOfDependencies; j++) {
          EditionReference dependency =
              SharedEditionPool->push<BlockType::Set>(1);
          if (Variables::HasVariable(exprChildDep->firstChild(),
                                     Parametric::k_localVariableId)) {
            /* Clone the entire parametric tree with detached dependency instead
             * of exprChild */
            expr->cloneNode();
            for (const Tree* exprChild2 : expr->children()) {
              if (exprChild2 != exprChild) {
                exprChild2->clone();
              } else {
                NAry::DetachChildAtIndex(exprChildDep, 0);
              }
            }
          } else {
            // Dependency can be detached out of parametric's scope.
            Variables::LeaveScope(NAry::DetachChildAtIndex(exprChildDep, 0));
          }
          MoveTreeBeforeNode(end, dependency);
          numberOfSets++;
        }
        exprChildDep->removeTree();
      } else {
        // Move dependency list at the end
        MoveTreeBeforeNode(end, exprChild->child(1));
        numberOfSets++;
      }
      // Remove Dependency block in child
      exprChild->removeNode();
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
