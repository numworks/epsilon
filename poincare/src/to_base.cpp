#include <poincare/to_base.h>
#include <ion.h>
#include <poincare/complex.h>
#include <poincare/undefined.h>
#include <poincare/rational.h>
#include "layout/char_layout.h"
#include "layout/horizontal_layout.h"

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type ToBase::type() const {
  return Type::ToBase;
}

Expression * ToBase::clone() const {
  ToBase * a = new ToBase(m_operands, true);
  return a;
}

int ToBase::polynomialDegree(char symbolName) const {
  return -1;
}

Expression * ToBase::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  Expression * op0 = editableOperand(0);
  Expression * op1 = editableOperand(1);
  if (op0->type() != Type::Rational || op1->type() != Type::Rational) {
    return replaceWith(new Undefined(), true);
  }
  Rational * r0 = static_cast<Rational *>(op0);
  Rational * r1 = static_cast<Rational *>(op1);

  // Grab base
  Integer intBase = r1->numerator();
  if (!r0->denominator().isOne() || !r1->denominator().isOne() || intBase.isLowerThan(Integer(2)) || Integer(36).isLowerThan(intBase)) {
    return replaceWith(new Undefined(), true);
  }
  int base;
  for (base = 1; !intBase.isEqualTo(Integer(base)); base++);

  Integer newNumerator(r0->numerator());
  newNumerator.setBase(base);
  return replaceWith(new Rational(newNumerator), true);
}

ExpressionLayout * ToBase::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != ComplexFormat::Default);
  HorizontalLayout * result = new HorizontalLayout();
  result->addOrMergeChildAtIndex(operand(0)->createLayout(floatDisplayMode, complexFormat), 0, false);
  result->addChildAtIndex(new CharLayout(Ion::Charset::Sto), result->numberOfChildren());
  result->addChildAtIndex(new CharLayout(':'), result->numberOfChildren());
  result->addOrMergeChildAtIndex(operand(1)->createLayout(floatDisplayMode, complexFormat), result->numberOfChildren(), false);
  return result;
}

int ToBase::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  int size = operand(0)->writeTextInBuffer(buffer, bufferSize, numberOfSignificantDigits);
  if (size + 5 > bufferSize) {
    return size;
  }
  buffer[size++] = Ion::Charset::Sto;
  buffer[size++] = ':';
  return size+operand(1)->writeTextInBuffer(buffer+size, bufferSize-size, numberOfSignificantDigits);
}

template<typename T>
Complex<T> ToBase::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  return c;
}

}

