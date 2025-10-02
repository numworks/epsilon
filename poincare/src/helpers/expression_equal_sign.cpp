#include <poincare/helpers/expression_equal_sign.h>
#include <poincare/src/expression/equal_sign.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/memory/tree.h>

namespace Poincare {

bool ExactAndApproximateLayoutsAreStrictlyEqual(
    const UserExpression exact, const UserExpression approximate,
    int numberOfSignificantDigits, const ProjectionContext& ctx) {
  ProjectionContext ctxCopy = ctx;
  ctxCopy.m_advanceReduce = false;
  // Exact is projected and reduced to turn divisions into rationals
  assert(!exact.isUninitialized());
  Internal::Tree* exactProjected = exact.tree()->cloneTree();
  if (!Internal::Simplification::Simplify(exactProjected, ctxCopy, false)) {
    exactProjected->removeTree();
    return false;
  }
  // Approximate is projected to turn Pow(e, …) into Exp(…)
  assert(!approximate.isUninitialized());
  Internal::Tree* approximateProjected = approximate.tree()->cloneTree();
  Internal::Simplification::ProjectAndReduce(approximateProjected, &ctxCopy);
  bool result = Internal::ExactAndApproximateLayoutsAreStrictlyEqual(
      exactProjected, approximateProjected, numberOfSignificantDigits);
  approximateProjected->removeTree();
  exactProjected->removeTree();
  return result;
}
}  // namespace Poincare
