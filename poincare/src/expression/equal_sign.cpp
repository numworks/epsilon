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
    return (!ctx.getTree(KA) || ctx.getTree(KA)->isDecimal()) &&
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

bool ExactAndApproximateExpressionsAreStrictlyEqual(const Tree* exact,
                                                    const Tree* approximate) {
  assert(exact && approximate);
  // If projection failed, exact can be not system
  assert(Simplification::IsSystem(approximate));

  /* Turn floats and doubles into decimal so that they can be compared to
   * rationals. */
  if (exact->isRational() && approximate->isFloat()) {
    /* Using parsing is the safest way to create a Decimal that will be the same
     * as what the user will read. */
    /* TODO: We need to be careful to the number of significant digits 1/16 ~=
     * 0.63 with 2 digits. But until now the app will call this with the
     * truncated float. */
    Tree* layout = Layouter::LayoutExpression(approximate);
    Layouter::StripSeparators(layout);
    Tree* parsed = Parser::Parse(layout);
    assert(isExpectedExpression(parsed));
    ProjectionContext ctx{.m_advanceReduce = false};
    Simplification::ProjectAndReduce(parsed, &ctx);
    bool result = exact->treeIsIdenticalTo(parsed);
    parsed->removeTree();
    layout->removeTree();
    return result;
  }

  if (exact->type() != approximate->type()) {
    // Ignore different types of undefined, except for NonReal
    return (approximate->isUndefined() && exact->isUndefined()) &&
           (approximate->isNonReal() == exact->isNonReal());
  }

  /* Check deeply for equality, because the expression can be a list, a matrix
   * or a complex composed of rationals.
   * Ex: 1 + i/2 == 1 + 0.5i */
  if (exact->numberOfChildren() != approximate->numberOfChildren()) {
    return false;
  }
  const Tree* approxChild = approximate->nextNode();
  for (const Tree* exactChild : exact->children()) {
    if (!ExactAndApproximateExpressionsAreStrictlyEqual(exactChild,
                                                        approxChild)) {
      return false;
    }
    approxChild = approxChild->nextTree();
  }

  return exact->nodeIsIdenticalTo(approximate);
}
}  // namespace Poincare::Internal
