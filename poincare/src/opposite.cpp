#include <poincare/opposite.h>
#include <poincare/complex_matrix.h>
#include <poincare/complex.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>
#include "layout/horizontal_layout.h"
#include "layout/parenthesis_layout.h"
#include "layout/string_layout.h"

namespace Poincare {

Opposite::Opposite(Expression * operand, bool cloneOperands) {
  assert(operand != nullptr);
  if (cloneOperands) {
    m_operand = operand->clone();
  } else {
    m_operand = operand;
  }
}

Opposite::~Opposite() {
  delete m_operand;
}

bool Opposite::hasValidNumberOfArguments() const {
  return m_operand->hasValidNumberOfArguments();
}

const Expression * Opposite::operand(int i) const {
  assert(i == 0);
  return m_operand;
}

int Opposite::numberOfOperands() const {
  return 1;
}

Expression * Opposite::clone() const {
  return this->cloneWithDifferentOperands((Expression**)&m_operand, 1, true);
}

Complex Opposite::compute(const Complex c) {
  return Complex::Cartesian(-c.a(), -c.b());
}

Evaluation * Opposite::computeOnMatrix(Evaluation * m) {
  Complex * operands = new Complex[m->numberOfRows() * m->numberOfColumns()];
  for (int i = 0; i < m->numberOfRows() * m->numberOfColumns(); i++) {
    Complex entry = *(m->complexOperand(i));
    operands[i] = Complex::Cartesian(-entry.a(), -entry.b());
  }
  Evaluation * matrix = new ComplexMatrix(operands, m->numberOfColumns(), m->numberOfRows());
  delete[] operands;
  return matrix;
}

Evaluation * Opposite::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation * operandEvalutation = m_operand->evaluate(context, angleUnit);
  if (operandEvalutation->numberOfRows() == 1 && operandEvalutation->numberOfColumns() == 1) {
    return new Complex(compute(*(operandEvalutation->complexOperand(0))));
  }
  return computeOnMatrix(operandEvalutation);
}

ExpressionLayout * Opposite::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout * children_layouts[2];
  char string[2] = {'-', '\0'};
  children_layouts[0] = new StringLayout(string, 1);
  children_layouts[1] = m_operand->type() == Type::Opposite ? new ParenthesisLayout(m_operand->createLayout(floatDisplayMode, complexFormat)) : m_operand->createLayout(floatDisplayMode, complexFormat);
  return new HorizontalLayout(children_layouts, 2);
}

Expression::Type Opposite::type() const {
  return Expression::Type::Opposite;
}

Expression * Opposite::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  assert(numberOfOperands == 1);
  return new Opposite(newOperands[0], cloneOperands);
}

}
