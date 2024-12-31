#include <poincare/helpers/expression_equal_sign.h>
#include <poincare/src/expression/equal_sign.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/memory/tree.h>

namespace Poincare {

bool ExactAndApproximateExpressionsAreStrictlyEqual(
    const UserExpression exact, const UserExpression approximate,
    const Internal::ProjectionContext* ctx) {
  Internal::ProjectionContext ctxCopy = *ctx;
  ctxCopy.m_advanceReduce = false;
  // Exact is projected and reduced to turn divisions into rationals
  Internal::Tree* exactProjected = exact.tree()->cloneTree();
  if (!Internal::Simplification::Simplify(exactProjected, ctxCopy, false)) {
    exactProjected->removeTree();
    return false;
  }
  // Approximate is projected to turn Pow(e, …) into Exp(…)
  Internal::Tree* approximateProjected = approximate.tree()->cloneTree();
  Internal::Simplification::ToSystem(approximateProjected, &ctxCopy);
  bool result = Internal::ExactAndApproximateExpressionsAreStrictlyEqual(
      exactProjected, approximateProjected);
  approximateProjected->removeTree();
  exactProjected->removeTree();
  return result;
}
}  // namespace Poincare
