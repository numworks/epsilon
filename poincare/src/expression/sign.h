#ifndef POINCARE_EXPRESSION_SIGN_H
#define POINCARE_EXPRESSION_SIGN_H

#include <poincare/sign.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

ComplexSign GetComplexSign(const Tree* e);

Sign GetSign(const Tree* e);

/* Sign of e1 - e2 so that e1 < e2 <=> SignOfDifference(e1, e2) < 0 and so on.
 * Beware that the difference may be real while the trees were complexes. */
ComplexSign ComplexSignOfDifference(const Tree* e1, const Tree* e2);

}  // namespace Poincare::Internal

#endif
