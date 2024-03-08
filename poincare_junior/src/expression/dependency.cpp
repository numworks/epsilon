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

bool ContainsSameDependency(const Tree* out, const Tree* in) {
  if (in->treeIsIdenticalTo(out)) {
    return true;
  }
  // TODO PCJ if power and same type of power return true
  for (const Tree* child : out->children()) {
    if (ContainsSameDependency(child, in)) {
      return true;
    }
  }
  return false;
}

bool RemoveUselessDependencies(Tree* dep) {
  Tree* expression = dep->child(0);
  Tree* list = dep->child(1);
  assert(list->isSet());
  for (int i = 0; i < list->numberOfChildren(); i++) {
    Tree* depI = list->child(i);
    // dep(..,{x*y}) = dep(..,{x+y}) = dep(..,{x ,y})
    if (depI->isAddition() || depI->isMultiplication()) {
      if (depI->numberOfChildren() == 1) {
        depI->moveTreeOverTree(depI->child(0));
      } else {
        NAry::AddChild(list, depI->child(0));
        NAry::SetNumberOfChildren(depI, depI->numberOfChildren() - 1);
      }
      i--;
      continue;
    }
    // dep(..,{x^y}) = dep(..,{x}) if y > 0 and y != p/2*q
    if (depI->isPower()) {
#if TODO_PCJ
      Power p = static_cast<Power&>(depI);
      if (p.typeOfDependency(reductionContext) == Power::DependencyType::None) {
        depI->moveTreeOverTree(depI->child(0));
        i--;
        continue;
      }
#endif
    }
  }

  // ShallowReduce to remove defined dependencies ({x+3}->{x, 3}->{x})
  // TODO PCJ

  /* Step 2: Remove duplicate dependencies and dependencies contained in others
   * {sqrt(x), sqrt(x), 1/sqrt(x)} -> {1/sqrt(x)} */
  for (int i = 0; i < list->numberOfChildren(); i++) {
    Tree* depI = list->child(i);
    for (int j = 0; j < list->numberOfChildren(); j++) {
      if (i == j) {
        continue;
      }
      if (ContainsSameDependency(list->child(j), depI)) {
        NAry::RemoveChildAtIndex(list, j);
        i--;
        break;
      }
    }
  }

  /* Step 3: Remove dependencies already contained in main expression.
   * dep(x^2+1,{x}) -> x^2+1 */
  for (int i = 0; i < list->numberOfChildren(); i++) {
    const Tree* depI = list->child(i);
    if (ContainsSameDependency(expression, depI)) {
      NAry::RemoveChildAtIndex(list, i);
      i--;
    }
  }

  return true;
}

bool Dependency::DeepRemoveUselessDependencies(Tree* expr) {
  bool changed = false;
  if (expr->isDependency()) {
    changed |= RemoveUselessDependencies(expr);
  }
  for (Tree* child : expr->children()) {
    changed |= DeepRemoveUselessDependencies(child);
  }
  return changed;
}

}  // namespace PoincareJ
