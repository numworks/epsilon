extern "C" {
#include <assert.h>
#include <stdlib.h>
}

#include <poincare/subtraction.h>
#include "layout/horizontal_layout.h"
#include "layout/string_layout.h"

Expression * Subtraction::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  assert(numberOfOperands == 2);
  return new Subtraction(newOperands, cloneOperands);
}

float Subtraction::approximate(Context& context) const {
  return m_operands[0]->approximate(context) - m_operands[1]->approximate(context);
}

Expression::Type Subtraction::type() const {
  return Expression::Type::Subtraction;
}

ExpressionLayout * Subtraction::createLayout() const {
  ExpressionLayout** children_layouts = (ExpressionLayout **)malloc(3*sizeof(ExpressionLayout *));
  children_layouts[0] = m_operands[0]->createLayout();
  char string[2] = {'-', '\0'};
  children_layouts[1] = new StringLayout(string, 1);
  children_layouts[2] = m_operands[1]->createLayout();
  return new HorizontalLayout(children_layouts, 3);
}

Expression * Subtraction::createEvaluation(Context& context) const {
  Expression * leftOperand = m_operands[0]->createEvaluation(context);
  Expression * rightOperand = m_operands[1]->createEvaluation(context);
  if (leftOperand == nullptr || rightOperand == nullptr) {
    return nullptr;
  }
  Expression * result = nullptr;
  if (leftOperand->type() == Expression::Type::Float && rightOperand->type() == Expression::Type::Float) {
    result = new Float(this->approximate(context));
  }
  if (leftOperand->type() == Expression::Type::Matrix && rightOperand->type() == Expression::Type::Float) {
    result = createEvaluationOnMatrixAndFloat((Matrix *)leftOperand, (Float *)rightOperand, context, true);
  }
  if (leftOperand->type() == Expression::Type::Float && rightOperand->type() == Expression::Type::Matrix) {
    result = createEvaluationOnMatrixAndFloat((Matrix *)rightOperand, (Float *)leftOperand, context, false);
  }
  if (leftOperand->type() == Expression::Type::Matrix && rightOperand->type() == Expression::Type::Matrix) {
    result = createEvaluationOnMatrices((Matrix *)leftOperand, (Matrix *)rightOperand, context);
  }
  delete leftOperand;
  delete rightOperand;
  return result;
}

Expression * Subtraction::createEvaluationOnMatrixAndFloat(Matrix * m, Float * a, Context& context, bool matrixMinusFloat) const {
  Expression * operands[m->numberOfRows() * m->numberOfColumns()];
  for (int i = 0; i < m->numberOfRows() * m->numberOfColumns(); i++) {
    if (matrixMinusFloat) {
      operands[i] = new Float(m->operand(i)->approximate(context) - a->approximate(context));
    } else {
      operands[i] = new Float(a->approximate(context) - m->operand(i)->approximate(context));
    }
  }
  return new Matrix(operands, m->numberOfRows() * m->numberOfColumns(), m->numberOfColumns(), m->numberOfRows(), false);
}

Expression * Subtraction::createEvaluationOnMatrices(Matrix * m, Matrix * n, Context& context) const {
  if (m->numberOfColumns() != n->numberOfColumns() || m->numberOfRows() != n->numberOfColumns()) {
    return nullptr;
  }
  Expression * operands[m->numberOfRows() * m->numberOfColumns()];
  for (int i = 0; i < m->numberOfRows() * m->numberOfColumns(); i++) {
    if (!m->operand(i)->approximate(context) || !n->operand(i)->approximate(context)) {
      return nullptr;
    }
    operands[i] = new Float(m->operand(i)->approximate(context) - n->operand(i)->approximate(context));
  }
  return new Matrix(operands, m->numberOfRows() * m->numberOfColumns(), m->numberOfColumns(), m->numberOfRows(), false);
}
