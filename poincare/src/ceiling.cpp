#include <poincare/ceiling.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Ceiling::Ceiling() :
  Function("ceil")
{
}

Expression::Type Ceiling::type() const {
  return Type::Ceiling;
}

Expression * Ceiling::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Ceiling * c = new Ceiling();
  c->setArgument(newOperands, numberOfOperands, cloneOperands);
  return c;
}

template<typename T>
Complex<T> Ceiling::templatedComputeComplex(const Complex<T> c) const {
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  return Complex<T>::Float(std::ceil(c.a()));
}

}


