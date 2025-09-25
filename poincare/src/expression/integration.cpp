#include "integration.h"

#include <poincare/k_tree.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

#include "advanced_reduction.h"
#include "dependency.h"
#include "parametric.h"
#include "poincare/src/expression/k_tree.h"
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
        remainingIntegrand->moveTreeOverTree(
            Integrate(symbol, a, b, remainingIntegrand, true));
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
      // Case of other variable has been escaped earlier
      assert(Variables::Id(integrand) == 0);
      // int(x, x, a, b) = 1/2 * (b^2 - a^2)
      return PatternMatching::CreateReduce(
          KMult(1_e / 2_e, KAdd(KPow(KB, 2_e), KMult(-1_e, KPow(KA, 2_e)))),
          {.KA = a, .KB = b});
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
    case Type::Trig: {
      PatternMatching::Context ctx;
      if (PatternMatching::Match(integrand->child(0), KMult(KA_s, KB), &ctx) &&
          Variables::IsVariableWithId(ctx.getTree(KB), 0)) {
        Tree* constant = PatternMatching::Create(KMult(KA_s), ctx);
        if (!Variables::HasVariable(constant, 0)) {
          /* int(sin(c*x), x, a, b) = 1/c * (cos(c*a) - cos(c*b))
           * int(cos(c*x), x, a, b) = 1/c * (sin(c*b) - sin(c*a)) */
          bool isSin = integrand->child(1)->isOne();
          TreeRef result = PatternMatching::CreateReduce(
              KMult(KPow(KC, -1_e),
                    KAdd(KTrig(KMult(KC, KA), KD),
                         KMult(-1_e, KTrig(KMult(KC, KB), KD)))),
              {
                  .KA = isSin ? a : b,
                  .KB = isSin ? b : a,
                  .KC = constant,
                  .KD = isSin ? 0_e : 1_e,
              });
          constant->removeTree();
          return result;
        }
      }
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
  static_assert(
      Parametric::k_variableIndex == 0 && Parametric::k_lowerBoundIndex == 1 &&
      Parametric::k_upperBoundIndex == 2 && Parametric::k_integrandIndex == 3);
  const Tree* symbol = e->child(0);
  const Tree* lowerBound = symbol->nextTree();
  const Tree* upperBound = lowerBound->nextTree();
  const Tree* integrand = upperBound->nextTree();
  /* Dependencies cannot be bubbled up outside of integrals because we need to
   * make sure the integrand is defined between bounds. Remove useless
   * dependencies here to still be able to reduce the integral if possible. */
  bool changed = Dependency::DeepRemoveUselessDependencies(e);
  Tree* integrandExpanded = integrand->cloneTree();
  /* Expand the integrand to improve output's approximation. This step could
   * rely on advanced reduction, or be moved in the multiplication case in
   * Integrate. */
  AdvancedReduction::DeepExpandAlgebraic(integrandExpanded);
  TreeRef result =
      Integrate(symbol, lowerBound, upperBound, integrandExpanded, false);
  integrandExpanded->removeTree();
  if (result) {
    // Add dependencies in bounds being real.
    SharedTreeStack->pushDepList(2);
    SharedTreeStack->pushReal();
    lowerBound->cloneTree();
    SharedTreeStack->pushReal();
    upperBound->cloneTree();
    result->cloneNodeAtNode(KDep);
    SystematicReduction::ShallowReduce(result->child(1));
    SystematicReduction::ShallowReduce(result);
    e->moveTreeOverTree(result);
    return true;
  }
  return changed;
}

}  // namespace Poincare::Internal
