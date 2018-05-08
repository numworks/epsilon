#include <poincare/complex_argument.h>
#include <poincare/complex.h>
#include <poincare/simplification_engine.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type ComplexArgument::type() const {
  return Type::ComplexArgument;
}

Expression * ComplexArgument::clone() const {
  ComplexArgument * a = new ComplexArgument(m_operands, true);
  return a;
}

Expression * ComplexArgument::shallowReduce(Context& context, AngleUnit angleUnit) {
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
  return this;
}

template<typename T>
Complex<T> ComplexArgument::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  return Complex<T>::Float(c.th());
}

}

