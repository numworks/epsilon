#include "integration.h"

#include <poincare/k_tree.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

#include "advanced_reduction.h"
#include "parametric.h"
#include "systematic_reduction.h"
#include "variables.h"

namespace Poincare::Internal {

static bool ExtractConstantFromIntegral(Tree* e) {
  assert(e->isIntegral());
  const Tree* integrand = e->child(Parametric::k_integrandIndex);

  if (!Variables::HasVariable(integrand, 0)) {
    // int(c, x, a, b) = c*(b-a) if c does not depend on x
    PatternMatching::MatchReplaceSimplify(e, KIntegral(KA, KB, KC, KD),
                                          KMult(KD, KAdd(KC, KMult(-1_e, KB))));
    return true;
  }

  if (integrand->isMult()) {
    // Separate the constant part of the integrand
    TreeRef constant = SharedTreeStack->pushMult(0);
    TreeRef remainingIntegrand = SharedTreeStack->pushMult(0);
    for (const Tree* child : integrand->children()) {
      Tree* childClone = child->cloneTree();
      if (Variables::HasVariable(childClone, 0)) {
        NAry::AddChild(remainingIntegrand, childClone);
      } else {
        NAry::AddChild(constant, childClone);
      }
    }
    NAry::SquashIfPossible(constant);
    NAry::SquashIfPossible(remainingIntegrand);

    if (constant->isOne()) {
      // No constant part
      remainingIntegrand->removeTree();
      constant->removeTree();
      return false;
    }

    // int(c * f(x), x, a, b) = c * int(f(x), x, a, b)
    assert(!remainingIntegrand->isOne());
    TreeRef remainingIntegral = e->cloneTree();
    remainingIntegral->child(Parametric::k_integrandIndex)
        ->moveTreeOverTree(remainingIntegrand);
    e->moveTreeOverTree(PatternMatching::CreateSimplify(
        KMult(KA, KB), {.KA = constant, .KB = remainingIntegral}));
    remainingIntegral->removeTree();
    constant->removeTree();
    return true;
  }

  return false;
}

bool Integration::Reduce(Tree* e) {
  assert(e->isIntegral());
  Tree* integrandExpanded = e->child(Parametric::k_integrandIndex)->cloneTree();
  AdvancedReduction::DeepExpandAlgebraic(integrandExpanded);

  if (integrandExpanded->isAdd()) {
    TreeRef splitIntegral =
        SharedTreeStack->pushAdd(integrandExpanded->numberOfChildren());
    for (const Tree* child : integrandExpanded->children()) {
      Tree* newChild = e->cloneTree();
      newChild->child(Parametric::k_integrandIndex)->cloneTreeOverTree(child);
      Integration::Reduce(newChild);
    }
    integrandExpanded->removeTree();
    SystematicReduction::ShallowReduce(splitIntegral);
    e->moveTreeOverTree(splitIntegral);
    return true;
  }
  integrandExpanded->removeTree();

  return ExtractConstantFromIntegral(e);
}

}  // namespace Poincare::Internal
