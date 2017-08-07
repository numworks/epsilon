extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>
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

Complex Power::compute(const Complex c, const Complex d) {
  if (d.b() != 0.0f) {
    /* First case c and d is complex */
    if (c.b() != 0.0f || c.a() <= 0) {
      return Complex::Float(NAN);
    }
    /* Second case only d is complex */
    float radius = std::pow(c.a(), d.a());
    float theta = d.b()*std::log(c.a());
    return Complex::Polar(radius, theta);
  }
  /* Third case only c is complex */
  float radius = std::pow(c.r(), d.a());
  if (c.b() == 0 && d.a() == std::round(d.a())) {
    /* We handle the case "c float and d integer" separatly to avoid getting
     * complex result due to float representation: a float power an integer is
     * always real. */
    return Complex::Cartesian(radius, 0.0f);
  }
  if (c.a() < 0 && c.b() == 0 && d.a() == 0.5f) {
    /* We handle the case "c negative float and d = 1/2" separatly to avoid
     * getting wrong result due to float representation: the squared root of
     * a negative float is always a pure imaginative. */
    return Complex::Cartesian(0.0f, radius);
  }
  /* Third case only c is complex */
  float theta = d.a()*c.th();
  return Complex::Polar(radius, theta);
}

Evaluation * Power::computeOnComplexMatrixAndComplex(Evaluation * m, const Complex * c) const {
 if (m->numberOfColumns() != m->numberOfRows()) {
    return new Complex(Complex::Float(NAN));
  }
  float power = c->toFloat();
  if (isnan(power) || isinf(power) || power != (int)power || std::fabs(power) > k_maxNumberOfSteps) {
    return new Complex(Complex::Float(NAN));
  }
  if (power < 0.0f) {
    Evaluation * inverse = m->createInverse();
    Complex minusC = Opposite::compute(*c);
    Evaluation * result = Power::computeOnComplexMatrixAndComplex(inverse, &minusC);
    delete inverse;
    return result;
  }
  Evaluation * result = ComplexMatrix::createIdentity(m->numberOfRows());
  // TODO: implement a quick exponentiation
  for (int k = 0; k < (int)power; k++) {
    if (shouldStopProcessing()) {
      delete result;
      return new Complex(Complex::Float(NAN));
    }
    result = Multiplication::computeOnMatrices(result, m);
  }
  return result;
}

Evaluation * Power::computeOnComplexAndComplexMatrix(const Complex * c, Evaluation * m) const {
  return new Complex(Complex::Float(NAN));
}

Evaluation * Power::computeOnNumericalMatrices(Evaluation * m, Evaluation * n) const {
  return new Complex(Complex::Float(NAN));
}

}
