#include <poincare/opposite.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <math.h>
}
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

Expression * Opposite::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  Expression * operandEvalutation = m_operand->evaluate(context, angleUnit);
  if (operandEvalutation == nullptr) {
    return nullptr;
  }
  Expression * result = nullptr;
  if (operandEvalutation->type() == Type::Complex) {
    result = new Complex(Complex::Cartesian(-((Complex *)operandEvalutation)->a(), -((Complex *)operandEvalutation)->b()));
  }
  if (operandEvalutation->type() == Type::Matrix) {
    result = evaluateOnMatrix((Matrix *)operandEvalutation, context, angleUnit);
  }
  delete operandEvalutation;
  return result;
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

float Opposite::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  return -m_operand->approximate(context, angleUnit);
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

Expression * Opposite::evaluateOnMatrix(Matrix * m, Context& context, AngleUnit angleUnit) const {
  Expression ** operands = (Expression **)malloc(m->numberOfRows() * m->numberOfColumns()*sizeof(Expression *));
  for (int i = 0; i < m->numberOfRows() * m->numberOfColumns(); i++) {
    Expression * evaluation = m->operand(i)->evaluate(context, angleUnit);
    assert(evaluation->type() == Type::Matrix || evaluation->type() == Type::Complex);
    if (evaluation->type() == Type::Matrix) {
      operands[i] = new Complex(Complex::Float(NAN));
      delete evaluation;
      continue;
    }
    operands[i] = new Complex(Complex::Cartesian(-((Complex *)evaluation)->a(), -((Complex *)evaluation)->b()));
    delete evaluation;
  }
  Expression * matrix = new Matrix(operands, m->numberOfRows() * m->numberOfColumns(), m->numberOfColumns(), m->numberOfRows(), false);
  free(operands);
  return matrix;
}

}
