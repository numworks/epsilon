#include "pdf_method.h"

#include <poincare/src/expression/infinity.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/rational.h>

namespace Poincare::Internal {

bool PDFMethod::shallowReduce(const Tree** abscissae,
                              const Distribution* distribution,
                              const Tree** parameters, Tree* expression) const {
  const Tree* x = abscissae[0];

  if (Infinity::TreeIsPlusOrMinusInfinity(x)) {
    expression->cloneTreeOverTree(0_e);
    return true;
  }

  if (!x->isRational()) {
    return false;
  }

  if (Rational::Sign(x).isStrictlyNegative() &&
      (distribution->hasType(Distribution::Type::Binomial) ||
       distribution->hasType(Distribution::Type::Poisson) ||
       distribution->hasType(Distribution::Type::Geometric) ||
       distribution->hasType(Distribution::Type::Hypergeometric))) {
    expression->cloneTreeOverTree(0_e);
    return true;
  }

  if (!distribution->isContinuous() && !x->isInteger()) {
    Tree* floorX = IntegerHandler::Quotient(Rational::Numerator(x),
                                            Rational::Denominator(x));
    // Replacing x in expression by its floor
    assert(x == expression->child(0));
    expression->child(0)->moveTreeOverTree(floorX);
    return true;
  }

  return false;
}

}  // namespace Poincare::Internal
