extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>
#include <math.h>
#include <poincare/complex_matrix.h>
#include <poincare/power.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include "layout/baseline_relative_layout.h"

namespace Poincare {

Expression::Type Power::type() const {
  return Type::Power;
}

Expression * Power::clone() const {
  return new Power(m_operands, true);
}

bool Power::isCommutative() const {
  return false;
}

template<typename T>
Complex<T> Power::compute(const Complex<T> c, const Complex<T> d) {
  if (d.b() != 0) {
    /* First case c and d is complex */
    if (c.b() != 0 || c.a() <= 0) {
      return Complex<T>::Float(NAN);
    }
    /* Second case only d is complex */
    T radius = std::pow(c.a(), d.a());
    T theta = d.b()*std::log(c.a());
    return Complex<T>::Polar(radius, theta);
  }
  /* Third case only c is complex */
  T radius = std::pow(c.r(), d.a());
  T theta = d.a()*c.th();
  return Complex<T>::Polar(radius, theta);
}

template<typename T> Evaluation<T> * Power::computeOnMatrixAndComplex(Evaluation<T> * m, const Complex<T> * d) {
 if (m->numberOfRows() != m->numberOfColumns()) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  T power = d->toScalar();
  if (isnan(power) || isinf(power) || power != (int)power || std::fabs(power) > k_maxNumberOfSteps) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  if (power < 0) {
    Evaluation<T> * inverse = m->createInverse();
    Complex<T> minusC = Opposite::compute(*d, AngleUnit::Default);
    Evaluation<T> * result = Power::computeOnMatrixAndComplex(inverse, &minusC);
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

template<typename T> Evaluation<T> * Power::computeOnComplexAndMatrix(const Complex<T> * c, Evaluation<T> * n) {
  return new Complex<T>(Complex<T>::Float(NAN));
}

template<typename T> Evaluation<T> * Power::computeOnMatrices(Evaluation<T> * m, Evaluation<T> * n) {

  return new Complex<T>(Complex<T>::Float(NAN));
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

}
