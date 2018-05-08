#include <poincare/cosine.h>
#include <poincare/hyperbolic_cosine.h>
#include <poincare/symbol.h>
#include <poincare/rational.h>
#include <poincare/multiplication.h>
#include <poincare/simplification_engine.h>
#include <ion.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type Cosine::type() const {
  return Type::Cosine;
}

Expression * Cosine::clone() const {
  Cosine * a = new Cosine(m_operands, true);
  return a;
}

float Cosine::characteristicXRange(Context & context, AngleUnit angleUnit) const {
  return Trigonometry::characteristicXRange(this, context, angleUnit);
}

template<typename T>
std::complex<T> Cosine::computeOnComplex(const std::complex<T> c, AngleUnit angleUnit) {
  std::complex<T> angleInput = Trigonometry::ConvertToRadian(c, angleUnit);
  std::complex<T> res = std::cos(angleInput);
  return Trigonometry::RoundToMeaningfulDigits(res);
}

Expression * Cosine::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
#if MATRIX_EXACT_REDUCING
  Expression * op = editableOperand(0);
  if (op->type() == Type::Matrix) {
    return SimplificationEngine::map(this, context, angleUnit);
  }
#endif
  return Trigonometry::shallowReduceDirectFunction(this, context, angleUnit);
}

}
