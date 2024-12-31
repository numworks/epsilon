#include "equal_sign.h"

#include <poincare/src/expression/simplification.h>
#include <poincare/src/layout/layouter.h>
#include <poincare/src/layout/parser.h>

namespace Poincare::Internal {

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
    Tree* layout = Layouter::LayoutExpression(approximate->cloneTree());
    Layouter::StripSeparators(layout);
    Tree* parsed = Parser::Parse(layout, nullptr);
    assert(parsed->isRationalOrFloat() || parsed->isDecimal() ||
           (parsed->isOpposite() && parsed->child(0)->isRationalOrFloat() ||
            parsed->child(0)->isDecimal()));
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

  /* The node content is ignored but should not matter, unless exact can be a
   * float. */
  assert(exact->nodeIsIdenticalTo(approximate));
  return true;
}
}  // namespace Poincare::Internal
