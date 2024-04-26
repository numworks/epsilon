#include "dependency.h"

#include <poincare/src/memory/block.h>
#include <poincare/src/memory/exception_checkpoint.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree_ref.h>

#include "approximation.h"
#include "k_tree.h"
#include "parametric.h"
#include "set.h"
#include "undefined.h"
#include "variables.h"

namespace Poincare::Internal {

bool Dependency::ShallowBubbleUpDependencies(Tree* expr) {
  if (expr->isDependency()) {
    if (!expr->child(0)->isDependency()) {
      return false;
    }
    Set::Union(expr->child(0)->child(1), expr->child(1));
    expr->removeNode();
    return true;
  }
  TreeRef finalSet = Set::PushEmpty();
  int i = 0;
  for (Tree* exprChild : expr->children()) {
    if (exprChild->isDependency()) {
      Tree* exprChildSet = exprChild->child(1);
      if (expr->isParametric() && Parametric::FunctionIndex(expr) == i) {
        if (expr->isDiff() || expr->isNthDiff()) {
          // diff(dep(x, {ln(x), z}), x, y) -> dep(diff(x, x, y), {ln(y), z})
          const Tree* symbolValue = expr->child(1);
          Variables::LeaveScopeWithReplacement(exprChildSet, symbolValue,
                                               false);
        } else {
          /* sum(dep(k, {f(k), z}), k, 1, n) ->
           *     dep(sum(k, k, 1, n), {sum(f(k), k, 1, n), z})
           * TODO:
           * - Keeping the dependency in the parametric would be more optimal,
           *   but we would have to handle them along the simplification process
           *   (especially difficult in the advanced and systematic reduction).
           */
          int numberOfDependencies = exprChildSet->numberOfChildren();
          TreeRef set = SharedTreeStack->push<Type::Set>(numberOfDependencies);
          for (int j = 0; j < numberOfDependencies; j++) {
            if (Variables::HasVariable(exprChildSet->firstChild(),
                                       Parametric::k_localVariableId)) {
              /* Clone the entire parametric tree with detached dependency
               * instead of exprChild */
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
      }
      // Move dependency list at the end
      Set::Union(finalSet, exprChildSet);
      // Remove Dependency block in child
      exprChild->removeNode();
    }
    i++;
  }
  if (finalSet->numberOfChildren() > 0) {
    expr->nextTree()->moveTreeBeforeNode(finalSet);
    expr->cloneNodeBeforeNode(KDep);
    return true;
  }
  return false;
};

bool Dependency::RemoveDefinedDependencies(Tree* dep) {
  /* TODO: We call this function to eliminate some dependencies after we
   * modified the list and it processes all the dependencies. We should rather
   * add an AddDependency method that makes sure the dependency is interesting
   * and not already covered by another one in the list. */
  Tree* set = dep->child(1);

  bool changed = false;
  int totalNumberOfDependencies = set->numberOfChildren();
  int i = 0;
  const Tree* depI = set->nextNode();
  while (i < totalNumberOfDependencies) {
    Tree* approximation;

    bool hasSymbolsOrRandom = depI->hasDescendantSatisfying([](const Tree* t) {
      return t->isUserNamed() || t->isVar() || t->isRandom();
    });
    if (hasSymbolsOrRandom) {
      /* If the dependency involves unresolved symbol/function/sequence, the
       * approximation of the dependency could be undef while the whole
       * expression is not. We just approximate everything but the symbol in
       * case the other parts of the expression make it undef/nonreal. */
      approximation = depI->clone();
      Approximation::ApproximateAndReplaceEveryScalar(approximation);
    } else {
      // TODO_PCJ
      approximation = Approximation::RootTreeToTree<double>(
          depI, AngleUnit::Radian, ComplexFormat::Real);
    }
    if (approximation->isUndef()) {
      Undefined::Type type = Undefined::GetType(approximation);
      approximation->removeTree();
      Undefined::Set(dep, type);
      return true;
    }
    // TODO_PR: Delete and handle with undefined type
    if (approximation->isNonReal()) {
      approximation->removeTree();
      dep->cloneTreeOverTree(KNonReal);
      return true;
    }
    approximation->removeTree();
    if (!hasSymbolsOrRandom) {
      changed = true;
      NAry::RemoveChildAtIndex(set, i);
      totalNumberOfDependencies--;
    } else {
      i++;
      depI = depI->nextTree();
    }
  }

  if (/*expression->isUndef() ||*/ totalNumberOfDependencies == 0) {
    set->removeTree();
    dep->removeNode();
    return true;
  }

  return changed;
}

bool ContainsSameDependency(const Tree* searched, const Tree* container) {
  // TODO handle scopes, x will not be seen in sum(k+x,k,1,n)
  if (searched->treeIsIdenticalTo(container)) {
    return true;
  }
  // TODO_PCJ if power and same type of power return true
  for (const Tree* child : container->children()) {
    if (ContainsSameDependency(searched, child)) {
      return true;
    }
  }
  return false;
}

bool RemoveUselessDependencies(Tree* dep) {
  const Tree* expression = dep->child(0);
  Tree* set = dep->child(1);
  // TODO: This function uses Set as an Nary which is an implementation detail
  assert(set->isSet());
  bool changed = false;
  Tree* depI = set->child(0);
  for (int i = 0; i < set->numberOfChildren(); i++) {
    // TODO is it true with infinite ? for instance -inf+inf is undef
    // dep(..,{x*y}) = dep(..,{x+y}) = dep(..,{x ,y})
    if (depI->isAdd() || depI->isMult()) {
      NAry::SetNumberOfChildren(
          set, set->numberOfChildren() + depI->numberOfChildren() - 1);
      depI->removeNode();
      i--;
      changed = true;
      continue;
    }
    // dep(..,{x^y}) = dep(..,{x}) if y > 0 and y != p/2*q
    if (depI->isPow()) {
#if TODO_PCJ
      Power p = static_cast<Power&>(depI);
      if (p.typeOfDependency(reductionContext) == Power::DependencyType::None) {
        depI->moveTreeOverTree(depI->child(0));
        i--;
        continue;
      }
#endif
    }
    depI = depI->nextTree();
  }
  if (changed) {
    NAry::Sort(set);
  }

  // ShallowReduce to remove defined dependencies ({x+3}->{x, 3}->{x})
  Dependency::RemoveDefinedDependencies(dep);
  if (!dep->isDependency()) {
    return true;
  }

  /* Step 2: Remove duplicate dependencies and dependencies contained in others
   * {sqrt(x), sqrt(x), 1/sqrt(x)} -> {1/sqrt(x)} */
  for (int i = 0; i < set->numberOfChildren(); i++) {
    Tree* depI = set->child(i);
    for (int j = 0; j < set->numberOfChildren(); j++) {
      if (i == j) {
        continue;
      }
      if (ContainsSameDependency(depI, set->child(j))) {
        NAry::RemoveChildAtIndex(set, i);
        i--;
        changed = true;
        break;
      }
    }
  }

  /* Step 3: Remove dependencies already contained in main expression.
   * dep(x^2+1,{x}) -> x^2+1 */
  depI = set->child(0);
  for (int i = 0; i < set->numberOfChildren(); i++) {
    if (ContainsSameDependency(depI, expression)) {
      NAry::RemoveChildAtIndex(set, i);
      i--;
      changed = true;
      continue;
    }
    depI = depI->nextTree();
  }

  changed |= Dependency::RemoveDefinedDependencies(dep);
  return changed;
}

bool Dependency::DeepRemoveUselessDependencies(Tree* expr) {
  bool changed = false;
  if (expr->isDependency()) {
    RemoveUselessDependencies(expr);
    return true;
  }
  for (Tree* child : expr->children()) {
    changed |= DeepRemoveUselessDependencies(child);
  }
  return changed;
}

}  // namespace Poincare::Internal
