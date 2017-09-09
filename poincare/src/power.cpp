extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>
#include <math.h>
#include <poincare/power.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include "layout/baseline_relative_layout.h"

namespace Poincare {

Expression::Type Power::type() const {
  return Type::Power;
}

Expression * Power::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 2);
  return new Power(newOperands, cloneOperands);
}

ExpressionLayout * Power::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  Expression * indiceOperand = m_operands[1];
  // Delete eventual parentheses of the indice in the pretty print
  if (m_operands[1]->type() == Type::Parenthesis) {
    indiceOperand = (Expression *)m_operands[1]->operand(0);
  }
  return new BaselineRelativeLayout(m_operands[0]->createLayout(floatDisplayMode, complexFormat),indiceOperand->createLayout(floatDisplayMode, complexFormat), BaselineRelativeLayout::Type::Superscript);
}

template<typename T>
Complex<T> Power::compute(const Complex<T> c, const Complex<T> d) {
  // c == c.r * e^(c.th*i)
  // d == d.a + d.b*i
  // c^d == e^(ln(c^d))
  //     == e^(ln(c) * d)
  //     == e^(ln(c.r * e^(c.th*i))  *  (d.a + d.b*i))
  //     == e^((ln(c.r) + ln(e^(c.th*i)))  *  (d.a + d.b*i))
  //     == e^((ln(c.r) + c.th*i)  *  (d.a + d.b*i))
  //     == e^(ln(c.r)*d.a + ln(c.r)*d.b*i + c.th*i*d.a + c.th*i*d.b*i)
  //     == e^((ln(c.r^d.a) + ln(e^(c.th*d.b*i^2)))  +  (ln(c.r)*d.b + c.th*d.a)*i)
  //     == e^(ln(c.r^d.a * e^(-c.th*d.b)))  *  e^((ln(c.r)*d.b + c.th*d.a)*i)
  //     == c.r^d.a*e^(-c.th*d.b) * e^((ln(c.r)*d.b + c.th*d.a)*i)
  T radius = std::pow(c.r(), d.a()) * std::exp(-c.th() * d.b());
  T theta = std::log(c.r())*d.b() + c.th()*d.a();
  return Complex<T>::Polar(radius, theta);
}

template<typename T> Evaluation<T> * Power::templatedComputeOnComplexMatrixAndComplex(Evaluation<T> * m, const Complex<T> * d) const {
 if (m->numberOfRows() != m->numberOfColumns()) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  T power = d->toScalar();
  if (isnan(power) || isinf(power) || power != (int)power || std::fabs(power) > k_maxNumberOfSteps) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  if (power < 0) {
    Evaluation<T> * inverse = m->createInverse();
    Complex<T> minusC = Opposite::compute(*d);
    Evaluation<T> * result = Power::computeOnComplexMatrixAndComplex(inverse, &minusC);
    delete inverse;
    return result;
  }
  Evaluation<T> * result = ComplexMatrix<T>::createIdentity(m->numberOfRows());
  // TODO: implement a quick exponentiation
  for (int k = 0; k < (int)power; k++) {
    if (shouldStopProcessing()) {
      delete result;
      return new Complex<T>(Complex<T>::Float(NAN));
    }
    result = Multiplication::computeOnMatrices(result, m);
  }
  return result;
}

template<typename T> Evaluation<T> * Power::templatedComputeOnComplexAndComplexMatrix(const Complex<T> * c, Evaluation<T> * n) const {
  return new Complex<T>(Complex<T>::Float(NAN));
}

template<typename T> Evaluation<T> * Power::templatedComputeOnComplexMatrices(Evaluation<T> * m, Evaluation<T> * n) const {

  return new Complex<T>(Complex<T>::Float(NAN));
}

template Complex<float> Power::compute<float>(Complex<float>, Complex<float>);
template Complex<double> Power::compute<double>(Complex<double>, Complex<double>);
}
