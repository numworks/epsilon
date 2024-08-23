#include "dependency.h"

#include <poincare/src/memory/block.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree_ref.h>

#include "approximation.h"
#include "k_tree.h"
#include "parametric.h"
#include "set.h"
#include "undefined.h"
#include "variables.h"

namespace Poincare::Internal {

bool Dependency::DeepBubbleUpDependencies(Tree* e) {
  return Tree::ApplyShallowBottomUp(
      e, [](Tree* e, void* context) { return ShallowBubbleUpDependencies(e); });
}

bool Dependency::ShallowBubbleUpDependencies(Tree* e) {
  if (e->isDep()) {
    bool changed = false;
    Tree* main = Main(e);
    // We cannot call Dependencies(e) here it might not be a DepList
    Tree* dependencies = e->child(k_dependenciesIndex);
    if (dependencies->isDep()) {
      assert(Main(dependencies)->isDepList());
      Set::Union(Main(dependencies), Dependencies(dependencies));
      dependencies->removeNode();
      changed = true;
    }
    dependencies = Dependencies(e);
    if (main->isDep()) {
      Set::Union(Dependencies(main), dependencies);
      e->removeNode();
      changed = true;
    }
    return changed;
  }
  TreeRef finalSet = KDepList()->cloneTree();
  int i = 0;
  for (Tree* child : e->children()) {
    if (child->isDep() && !Undefined::CanHaveUndefinedChild(e, i)) {
      Tree* childSet = Dependencies(child);
      /* TODO_PCJ: bubble up across list operators in the same fashion as on
       * parametrics
       * */
      if (e->isParametric() && Parametric::FunctionIndex(e) == i) {
        if (e->isDiff()) {
          // diff(dep({ln(x), z}, x), x, y) -> dep({ln(y), z}, diff(x, x, y))
          const Tree* symbolValue = e->child(1);
          Variables::LeaveScopeWithReplacement(childSet, symbolValue, false);
        } else {
          /* sum(dep({    f(k),           z},     k), k, 1, n) ->
           *     dep({sum(f(k), k, 1, n), z}, sum(k,  k, 1, n))
           * TODO:
           * - Keeping the dependency in the parametric would be more optimal,
           *   but we would have to handle them along the simplification process
           *   (especially difficult in the advanced and systematic reduction).
           */
          int numberOfDependencies = childSet->numberOfChildren();
          TreeRef set = SharedTreeStack->pushDepList(numberOfDependencies);
          for (int j = 0; j < numberOfDependencies; j++) {
            if (Variables::HasVariable(childSet->child(0),
                                       Parametric::k_localVariableId)) {
              /* Clone the entire parametric tree with detached dependency
               * instead of child */
              e->cloneNode();
              for (const Tree* child2 : e->children()) {
                if (child2 != child) {
                  child2->cloneTree();
                } else {
                  NAry::DetachChildAtIndex(childSet, 0);
                }
              }
            } else {
              // Dependency can be detached out of parametric's scope.
              Variables::LeaveScope(NAry::DetachChildAtIndex(childSet, 0));
            }
          }
          childSet->removeTree();
          childSet = set;
        }
      }
      // Move dependency list at the end
      Set::Union(finalSet, childSet);
      // Remove Dependency block in child
      child->removeNode();
    }
    i++;
  }
  if (finalSet->numberOfChildren() > 0) {
    e->nextTree()->moveTreeBeforeNode(finalSet);
    e->cloneNodeBeforeNode(KDep);
    return true;
  }
  finalSet->removeTree();
  return false;
};

bool RemoveDefinedDependencies(Tree* dep) {
  /* TODO: We call this function to eliminate some dependencies after we
   * modified the list and it processes all the dependencies. We should rather
   * add an AddDependency method that makes sure the dependency is interesting
   * and not already covered by another one in the list. */
  Tree* set = Dependency::Dependencies(dep);

  bool changed = false;
  int totalNumberOfDependencies = set->numberOfChildren();
  int i = 0;
  const Tree* depI = set->nextNode();
  while (i < totalNumberOfDependencies) {
    if (!Undefined::CanBeUndefined(depI)) {
      changed = true;
      NAry::RemoveChildAtIndex(set, i);
      totalNumberOfDependencies--;
      continue;
    }
    Tree* approximation = depI->cloneTree();
    // TODO_PCJ: Ensure the default Radian/Cartesian context is good enough.
    Approximation::ApproximateAndReplaceEveryScalar(approximation);
    if (approximation->isUndefined()) {
      dep->moveTreeOverTree(approximation);
      return true;
    }

    bool canBeApproximated = Approximation::CanApproximate(approximation);
    /* TODO: depI could be replaced with approximation anyway, but we need the
     * exact dependency for ContainsSameDependency, used later one. */
    approximation->removeTree();
    if (canBeApproximated) {
      changed = true;
      NAry::RemoveChildAtIndex(set, i);
      totalNumberOfDependencies--;
    } else {
      i++;
      depI = depI->nextTree();
    }
  }

  // e->isUndefined() ||
  if (totalNumberOfDependencies == 0) {
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
  if (((container->isLn() && searched->isPow() &&
        searched->child(0)->treeIsIdenticalTo(container->child(0)) &&
        searched->child(1)->isStrictlyNegativeInteger()) ||
       (searched->isLn() && container->isPow() &&
        container->child(0)->treeIsIdenticalTo(searched->child(0)) &&
        container->child(1)->isStrictlyNegativeInteger()) ||
       (searched->isLn() && container->isLnReal()) ||
       (searched->isPow() && container->isPowReal() &&
        searched->child(1)->treeIsIdenticalTo(container->child(1)))) &&
      searched->child(0)->treeIsIdenticalTo(container->child(0))) {
    /* x^-n contains ln(x) and inversely
     * lnReal(x) contains ln(x)
     * powReal(x,y) contains pow(x,y) */
    return true;
  }
  if (searched->isPow() && container->isPow() &&
      searched->child(0)->treeIsIdenticalTo(container->child(0)) &&
      searched->child(1)->isMinusOne() && container->child(1)->isZero()) {
    /* x^0 contains x^-1 */
    /* TODO_PCJ
     * - add other possibilities (like x^1/4 contains x^1/2, x^-n contains x^n
     *   with n positive integer)
     * - with PowReal */
    return true;
  }
  for (const Tree* child : container->children()) {
    if (ContainsSameDependency(searched, child)) {
      return true;
    }
  }
  return false;
}

// These expression are only undef if their child is undef.
bool IsDefinedIfChildIsDefined(const Tree* e) {
  /* TODO_PCJ: For better sensitivity (simplify more dependencies):
   *           - Use the old Power::typeOfDependency logic :
   *                dep(..,{x^y}) = dep(..,{x}) if y > 0 and y != p/2*q
   *           - Handle logarithm of non null children */
  return e->isOfType({Type::Trig, Type::Abs, Type::Exp, Type::Re, Type::Im,
                      Type::ATrig, Type::ATanRad}) ||
         (e->isPow() && e->child(1)->isStrictlyPositiveInteger());
}

bool ShallowRemoveUselessDependencies(Tree* dep) {
  const Tree* expression = Dependency::Main(dep);
  Tree* set = Dependency::Dependencies(dep);
  // TODO: This function uses Set as an Nary which is an implementation detail
  assert(set->isDepList());
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
    if (IsDefinedIfChildIsDefined(depI)) {
      // dep(..., {f(x)}) = dep(..., {x}) with f always defined if x defined
      depI->moveTreeOverTree(depI->child(0));
      i--;
      changed = true;
      continue;
    }
    // TODO: pow(x,n) -> x with n positive integer?
    depI = depI->nextTree();
  }
  if (changed) {
    NAry::Sort(set);
  }

  // ShallowReduce to remove defined dependencies ({x+3}->{x, 3}->{x})
  RemoveDefinedDependencies(dep);
  if (!dep->isDep()) {
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

  changed |= RemoveDefinedDependencies(dep);
  return changed;
}

bool Dependency::DeepRemoveUselessDependencies(Tree* e) {
  bool changed = false;
  for (Tree* child : e->children()) {
    changed |= DeepRemoveUselessDependencies(child);
  }
  if (e->isDep()) {
    changed |= ShallowRemoveUselessDependencies(e);
  }
  return changed;
}

bool Dependency::RemoveDependencies(Tree* e) {
  if (e->isDep()) {
    e->moveTreeOverTree(Main(e));
    return true;
  }
  return false;
}

}  // namespace Poincare::Internal
