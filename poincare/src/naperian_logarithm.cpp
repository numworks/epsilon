#include <poincare/naperian_logarithm.h>
#include <poincare/symbol.h>
#include <poincare/logarithm.h>
#include <poincare/simplification_engine.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <ion.h>
#include <cmath>

namespace Poincare {

Expression::Type NaperianLogarithm::type() const {
  return Type::NaperianLogarithm;
}

Expression * NaperianLogarithm::clone() const {
  NaperianLogarithm * a = new NaperianLogarithm(m_operands, true);
  return a;
}

Expression * NaperianLogarithm::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
#if MATRIX_EXACT_REDUCING
  if (operand(0)->type() == Type::Matrix) {
    return SimplificationEngine::map(this, context, angleUnit);
  }
#endif
  const Expression * logOperands[2] = {operand(0)->clone(), new Symbol(Ion::Charset::Exponential)};
  Logarithm * l = new Logarithm(logOperands, 2, false);
  replaceWith(l, true);
  return l->shallowReduce(context, angleUnit);
}

template<typename T>
std::complex<T> NaperianLogarithm::computeOnComplex(const std::complex<T> c, AngleUnit angleUnit) {
  /* ln has a branch cut on ]-inf, 0]: it is then multivalued on this cut. We
   * followed the convention chosen by the lib c++ of llvm on ]-inf+0i, 0+0i]
   * (warning: ln takes the other side of the cut values on ]-inf-0i, 0-0i]). */
  return std::log(c);
}

}
