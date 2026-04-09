#include "equal_sign.h"

#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/layout/layouter.h>
#include <poincare/src/layout/parser.h>
#include <poincare/src/memory/pattern_matching.h>

namespace Poincare::Internal {

bool isExpectedExpression(const Tree* expr) {
  if (PatternMatching::Context ctx;
      PatternMatching::Match(expr, KMult(KA, KPow(10_e, KB)), &ctx) ||
      PatternMatching::Match(expr, KPow(10_e, KB), &ctx)) {
    return (!ctx.getTree(KA) ||
            (ctx.getTree(KA)->isDecimal() || ctx.getTree(KA)->isInteger())) &&
           (ctx.getTree(KB)->isInteger() ||
            (ctx.getTree(KB)->isOpposite() &&
             ctx.getTree(KB)->child(0)->isInteger()));
  }
  /* Warning this matches a larger class of expression that the actual float
   * layout outputs. Is the assert useful at all ? */
  return expr->isRationalOrFloat() || expr->isDecimal() ||
         ((expr->isOpposite() || expr->isParentheses()) &&
          isExpectedExpression(expr->child(0)));
}

bool RecursiveIsCalculationOutputStrictEquality(const Tree* exact,
                                                const Tree* approximate,
                                                int significantDigits) {
  assert(exact && approximate);
  if (exact->isUndefined() || approximate->isUndefined()) {
    // We never want to display "≈ undef" nor "undef ≈"
    return true;
  }

  /* Turn floats and doubles into decimal so that they can be compared to
   * rationals. */
  if (exact->isRational() && approximate->isFloat()) {
    /* Using parsing is the safest way to create a Decimal that will be the same
     * as what the user will read. */
    Tree* layout = Layouter::LayoutExpression(
        approximate, {.numberOfSignificantDigits = significantDigits});
    Layouter::StripSeparators(layout);
    Tree* parsed = Parser::UnsafeParse(layout);
    assert(isExpectedExpression(parsed));
    Simplification::ProjectAndReduce(
        parsed,
        SimplificationParameters{.reductionPolicy = {.advanced = false}});
    bool result = exact->treeIsIdenticalTo(parsed);
    parsed->removeTree();
    layout->removeTree();
    return result;
  }

  if (!exact->nodeIsIdenticalTo(approximate)) {
    return false;
  }
  assert(exact->numberOfChildren() == approximate->numberOfChildren());
  /* Check deeply for equality, because the expression can be a list, a matrix
   * or a complex composed of rationals.
   * Ex: 1 + i/2 == 1 + 0.5i */
  const Tree* approxChild = approximate->nextNode();
  for (const Tree* exactChild : exact->children()) {
    if (!RecursiveIsCalculationOutputStrictEquality(exactChild, approxChild,
                                                    significantDigits)) {
      return false;
    }
    approxChild = approxChild->nextTree();
  }
  return true;
}

bool IsCalculationOutputStrictEquality(const Tree* exact,
                                       const Tree* approximate,
                                       int significantDigits) {
  assert(exact && approximate);
  // If projection failed, exact can be not system
  assert(Simplification::CanBeSystem(approximate));
  if (exact->hasDescendantSatisfying(
          [](const Tree* e) { return e->isUserSequence(); })) {
    /* Sequence are always approximated, but we want a strict equality sign for
     * the approximate result to prevent U_3 ≈ 12. */
    return true;
  }
  return RecursiveIsCalculationOutputStrictEquality(exact, approximate,
                                                    significantDigits);
}

}  // namespace Poincare::Internal
