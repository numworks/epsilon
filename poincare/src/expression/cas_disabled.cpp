#include <poincare/preferences.h>
#include <poincare/src/memory/tree.h>

#include "cas.h"
#include "dimension.h"
#include "units/unit.h"

namespace Poincare::Internal {

namespace {

bool isPrimeFactorization(const Tree* expression) {
  /* A prime factorization can only be built with integers, powers of integers,
   * and a multiplication. */
  for (const Tree* t : expression->selfAndDescendants()) {
    if (!t->isInteger() && !t->isMult() &&
        !(t->isPow() && t->child(0)->isInteger() && t->child(1)->isInteger())) {
      return false;
    }
  }
  return true;
}

bool exactExpressionIsForbidden(const Tree* exactOutput) {
  if (!Preferences::SharedPreferences()->examMode().forbidExactResults()) {
    return false;
  }
  if (exactOutput->isOpposite()) {
    return exactExpressionIsForbidden(exactOutput->child(0));
  }
  bool isFraction = exactOutput->isDiv() && exactOutput->child(0)->isNumber() &&
                    exactOutput->child(1)->isNumber();
  return !(exactOutput->isNumber() || isFraction ||
           isPrimeFactorization(exactOutput));
}

bool neverDisplayExactOutput(const Tree* exactOutput, Context* context) {
  if (!exactOutput) {
    return false;
  }
  /* Force all outputs to be ApproximateOnly if required by the exam mode
   * configuration */
  if (exactExpressionIsForbidden(exactOutput)) {
    return true;
  }
  bool allChildrenAreUndefined = exactOutput->numberOfChildren() > 0;
  /* 1. If the output contains a comparison, we only display the
   * approximate output. (this can occur for pi > 3 for example, since
   * it's handled by approximation and not by reduction)
   * 2. If the output has remaining depencies, the exact output is not
   * displayed to avoid outputs like 5 ≈ undef and also because it could
   * be a reduction that failed and was interrupted which can lead to
   * dependencies not being properly bubbled-up */
  for (const Tree* t : exactOutput->selfAndDescendants()) {
    if (t->isComparison() || t->isDep()) {
      return true;
    }
    if (!t->isUndefined()) {
      allChildrenAreUndefined = false;
    }
  }
  if
      // Lists or Matrices with only nonreal/undefined children
      (allChildrenAreUndefined &&
       (exactOutput->isList() || exactOutput->isMatrix())) {
    return true;
  }

  Internal::Dimension d = Internal::Dimension::Get(exactOutput, context);
  // Angle units can have an exact output contrary to other units
  return d.isUnit() && !d.isAngleUnit();
}

bool neverDisplayExactExpressionOfApproximation(const Tree* approximateOutput,
                                                Context* context) {
  /* The angle units could display exact output but we want to avoid exact
   * results that are not in radians like "(3/sqrt(2))°" because they are not
   * relevant for the user.
   * On the other hand, we'd like "cos(4°)" to be displayed as exact result.
   * To do so, the approximateOutput is checked rather than the exactOutput,
   * because the approximateOutput has a unit only if the degree unit is not
   * in a trig function. */
  Internal::Dimension d = Internal::Dimension::Get(approximateOutput, context);
  return d.isUnit() && !d.isSimpleRadianAngleUnit();
}

}  // namespace

bool CAS::Enabled() { return false; }

bool CAS::NeverDisplayReductionOfInput(const Internal::Tree* input,
                                       Context* context) {
  if (!input) {
    return false;
  }
  return input->hasDescendantSatisfying([](const Tree* t) {
    return t->isOfType({
               Type::PhysicalConstant,
               Type::RandInt,
               Type::RandIntNoRep,
               Type::Random,
               Type::Round,
               Type::Frac,
               Type::Integral,
               Type::Product,
               Type::Sum,
               Type::Diff,
               Type::Distribution,
           }) ||
           t->isUserSequence();
  });
}

bool CAS::ShouldOnlyDisplayApproximation(
    const Internal::Tree* input, const Internal::Tree* exactOutput,
    const Internal::Tree* approximateOutput, Context* context) {
  return NeverDisplayReductionOfInput(input, context) ||
         neverDisplayExactOutput(exactOutput, context) ||
         (approximateOutput && neverDisplayExactExpressionOfApproximation(
                                   approximateOutput, context));
}

}  // namespace Poincare::Internal
