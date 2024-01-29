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
      Tree* exprChildSet = exprChild->child(1);
      if (expr->isParametric() && Parametric::FunctionIndex(expr) == i) {
        /* diff(dep(x, {ln(x), z}), x, y) ->
         * dep(diff(x, x, y), {diff(ln(x), x, y), z})
         * TODO:
         * - Keeping the dependency in the parametric would be more optimal, but
         *   we would have to handle them along the simplification process
         *   (especially difficult in the advanced and systematic reduction).
         * - In the case of derivatives only, we could simply bubble up
         *   dependency and replace local variable with symbol value. */
        int numberOfDependencies = exprChildSet->numberOfChildren();
        EditionReference set =
            SharedEditionPool->push<BlockType::Set>(numberOfDependencies);
        for (int j = 0; j < numberOfDependencies; j++) {
          if (Variables::HasVariable(exprChildSet->firstChild(),
                                     Parametric::k_localVariableId)) {
            /* Clone the entire parametric tree with detached dependency instead
             * of exprChild */
            expr->cloneNode();
            for (const Tree* exprChild2 : expr->children()) {
              if (exprChild2 != exprChild) {
                exprChild2->clone();
              } else {
                NAry::DetachChildAtIndex(exprChildSet, 0);
              }
            }
          } else {
            // Dependency can be detached out of parametric's scope.
            Variables::LeaveScope(NAry::DetachChildAtIndex(exprChildSet, 0));
          }
        }
        exprChildSet->removeTree();
        exprChildSet = set;
      }
      // Move dependency list at the end
      MoveTreeBeforeNode(end, exprChildSet);
      numberOfSets++;
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
