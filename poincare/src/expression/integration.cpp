#include "integration.h"

#include <poincare/k_tree.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

#include "advanced_reduction.h"
#include "parametric.h"
#include "systematic_reduction.h"
#include "variables.h"

namespace Poincare::Internal {

static Tree* Integrate(const Tree* symbol, const Tree* a, const Tree* b,
                       const Tree* integrand, bool force) {
  if (!Variables::HasVariable(integrand, 0)) {
    // int(c, x, a, b) = c*(b-a) if c does not depend on x
    return PatternMatching::CreateSimplify(KMult(KC, KAdd(KB, KMult(-1_e, KA))),
                                           {.KA = a, .KB = b, .KC = integrand});
  }
  if (integrand->isAdd()) {
    Tree* result = SharedTreeStack->pushAdd(integrand->numberOfChildren());
    for (const Tree* child : integrand->children()) {
      Integrate(symbol, a, b, child, true);
    }
    SystematicReduction::ShallowReduce(result);
    return result;
  }
  static_assert(
      Parametric::k_variableIndex == 0 && Parametric::k_lowerBoundIndex == 1 &&
      Parametric::k_upperBoundIndex == 2 && Parametric::k_integrandIndex == 3);
  if (integrand->isMult()) {
    // Separate the constant part of the integrand
    TreeRef constant = SharedTreeStack->pushMult(0);
    TreeRef remainingIntegrand = SharedTreeStack->pushMult(0);
    for (const Tree* child : integrand->children()) {
      NAry::AddChild(
          Variables::HasVariable(child, 0) ? remainingIntegrand : constant,
          child->cloneTree());
    }
    if (constant->numberOfChildren() > 0) {
      assert(remainingIntegrand->numberOfChildren() > 0);
      assert(constant->nextTree() == remainingIntegrand);
      // int(c * f(x), x, a, b) = c * int(f(x), x, a, b)
      NAry::SquashIfUnary(remainingIntegrand);
      assert(!SystematicReduction::ShallowReduce(remainingIntegrand));
      (KIntegral)->cloneNode();
      symbol->cloneTree();
      a->cloneTree();
      b->cloneTree();
      remainingIntegrand->detachTree();
      NAry::SetNumberOfChildren(constant, constant->numberOfChildren() + 1);
      SystematicReduction::ShallowReduce(constant);
      return constant;
    }
    // No constant part, fall back to default case
    remainingIntegrand->removeTree();
    constant->removeTree();
  }
  // Not handled
  return force
             ? PatternMatching::Create(
                   KIntegral(KA, KB, KC, KD),
                   {.KA = symbol, .KB = a, .KC = b, .KD = integrand}, {.KD = 1})
             : nullptr;
}

bool Integration::Reduce(Tree* e) {
  assert(e->isIntegral());
  Tree* integrandExpanded = e->child(Parametric::k_integrandIndex)->cloneTree();
  /* Expand the integrand to improve output's approximation. This step could
   * rely on advanced reduction, or be moved in the multiplication case in
   * Integrate. */
  AdvancedReduction::DeepExpandAlgebraic(integrandExpanded);
  TreeRef result = Integrate(e->child(Parametric::k_variableIndex),
                             e->child(Parametric::k_lowerBoundIndex),
                             e->child(Parametric::k_upperBoundIndex),
                             integrandExpanded, false);
  integrandExpanded->removeTree();
  if (result) {
    e->moveTreeOverTree(result);
    return true;
  }
  return false;
}

}  // namespace Poincare::Internal
