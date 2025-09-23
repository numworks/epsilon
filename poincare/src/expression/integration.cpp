#include "integration.h"

#include <poincare/k_tree.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

#include "advanced_reduction.h"
#include "dependency.h"
#include "parametric.h"
#include "systematic_reduction.h"
#include "variables.h"

namespace Poincare::Internal {

static Tree* Integrate(const Tree* symbol, const Tree* a, const Tree* b,
                       const Tree* integrand, bool force) {
  if (!Variables::HasVariable(integrand, 0)) {
    // int(c, x, a, b) = c*(b-a) if c does not depend on x
    Tree* result = integrand->cloneTree();
    Variables::LeaveScope(result);
    result->moveTreeOverTree(
        PatternMatching::CreateReduce(KMult(KC, KAdd(KB, KMult(-1_e, KA))),
                                      {.KA = a, .KB = b, .KC = result}));
    return result;
  }
  switch (integrand->type()) {
    case Type::Add: {
      Tree* result = SharedTreeStack->pushAdd(integrand->numberOfChildren());
      for (const Tree* child : integrand->children()) {
        Integrate(symbol, a, b, child, true);
      }
      SystematicReduction::ShallowReduce(result);
      return result;
    }
    case Type::Mult: {
      static_assert(Parametric::k_variableIndex == 0 &&
                    Parametric::k_lowerBoundIndex == 1 &&
                    Parametric::k_upperBoundIndex == 2 &&
                    Parametric::k_integrandIndex == 3);
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
        Variables::LeaveScope(constant);
        NAry::SquashIfUnary(remainingIntegrand);
        assert(!SystematicReduction::ShallowReduce(remainingIntegrand));
        TreeRef integral = (KIntegral)->cloneNode();
        symbol->cloneTree();
        a->cloneTree();
        b->cloneTree();
        remainingIntegrand->detachTree();
        Integration::Reduce(integral);
        NAry::SetNumberOfChildren(constant, constant->numberOfChildren() + 1);
        SystematicReduction::ShallowReduce(constant);
        return constant;
      }
      // No constant part, fall back to default case
      remainingIntegrand->removeTree();
      constant->removeTree();
      break;
    }
    case Type::Var: {
      if (Variables::Id(integrand) == 0) {
        // int(x, x, a, b) = 1/2 * (b^2 - a^2)
        return PatternMatching::CreateReduce(
            KMult(1_e / 2_e, KAdd(KPow(KB, 2_e), KMult(-1_e, KPow(KA, 2_e)))),
            {.KA = a, .KB = b});
      }
      break;
    }
    case Type::Pow: {
      const Tree* integrandBase = integrand->child(0);
      const Tree* integrandExp = integrandBase->nextTree();
      if (integrandExp->isPositiveInteger() &&
          Variables::IsVariableWithId(integrandBase, 0)) {
        /* int(x^n, x, a, b) = 1/(n+1) * (b^(n+1) - a^(n+1))
         * if n is a positive integer */
        return PatternMatching::CreateReduce(
            KMult(KPow(KAdd(KC, 1_e), -1_e),
                  KAdd(KPow(KB, KAdd(KC, 1_e)),
                       KMult(-1_e, KPow(KA, KAdd(KC, 1_e))))),
            {.KA = a, .KB = b, .KC = integrandExp});
      }
      break;
    }
    default:
      break;
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
  /* Dependencies cannot be bubbled up outside of integrals because we need to
   * make sure the integrand is defined between bounds. Remove useless
   * dependencies here to still be able to reduce the integral if possible. */
  bool changed = Dependency::DeepRemoveUselessDependencies(e);
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
  return changed;
}

}  // namespace Poincare::Internal
