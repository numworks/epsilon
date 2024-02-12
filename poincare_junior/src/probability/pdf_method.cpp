#include "pdf_method.h"

#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/rational.h>

namespace PoincareJ {

bool PDFMethod::shallowReduce(const Tree** abscissae,
                              const Distribution* distribution,
                              const Tree** parameters, Tree* expression) const {
  const Tree* x = abscissae[0];

  // TODO PCJ: -inf
  if (x->isInfinity()) {
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

  if (!distribution->isContinuous()) {
#if 0
    // TODO PCJ: replace x
    Tree* div = IntegerHandler::Quotient(Rational::Numerator(x),
                                         Rational::Denominator(x));
    x->moveTreeOverTree(div);
    return true;
#endif
  }

  return false;
}

}  // namespace PoincareJ
