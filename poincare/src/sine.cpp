#include <poincare/sine.h>
#include <poincare/trigonometry.h>
#include <poincare/hyperbolic_sine.h>
#include <poincare/multiplication.h>
#include <poincare/symbol.h>
#include <poincare/simplification_engine.h>
#include <ion.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type Sine::type() const {
  return Expression::Type::Sine;
}

Expression * Sine::clone() const {
  Sine * a = new Sine(m_operands, true);
  return a;
}

float Sine::characteristicXRange(Context & context, AngleUnit angleUnit) const {
  return Trigonometry::characteristicXRange(this, context, angleUnit);
}

template<typename T>
std::complex<T> Sine::computeOnComplex(const std::complex<T> c, AngleUnit angleUnit) {
  std::complex<T> angleInput = Trigonometry::ConvertToRadian(c, angleUnit);
  std::complex<T> res = std::sin(angleInput);
  return Trigonometry::RoundToMeaningfulDigits(res);
}

Expression * Sine::shallowReduce(Context& context, AngleUnit angleUnit) {
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
