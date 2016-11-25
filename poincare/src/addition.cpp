#include <poincare/addition.h>
#include <poincare/expression.h>
#include "layout/horizontal_layout.h"
#include <assert.h>

Expression::Type Addition::type() const {
  return Expression::Type::Addition;
}

Expression * Addition::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  return new Addition(newOperands, numberOfOperands, cloneOperands);
}

float Addition::operateApproximatevelyOn(float a, float b) const {
  return a + b;
}

Expression * Addition::createEvaluationOn(Expression * a, Expression * b, Context& context) const {
  if (a == nullptr || b == nullptr) {
    return nullptr;
  }
  assert(a->type() == Expression::Type::Float || a->type() == Expression::Type::Matrix);
  assert(b->type() == Expression::Type::Float || b->type() == Expression::Type::Matrix);
  Expression * result = nullptr;
  if (a->type() == Expression::Type::Float && b->type() == Expression::Type::Float) {
    result = new Float(a->approximate(context) + b->approximate(context));
  }
  if (a->type() == Expression::Type::Float && b->type() == Expression::Type::Matrix) {
    result = createEvaluationOnFloatAndMatrix((Float *)a, (Matrix *)b, context);
  }
  if (b->type() == Expression::Type::Float && a->type() == Expression::Type::Matrix) {
    result = createEvaluationOnFloatAndMatrix((Float *)b, (Matrix *)a, context);
  }
  if (b->type() == Expression::Type::Matrix && a->type() == Expression::Type::Matrix) {
    result = createEvaluationOnMatrices((Matrix *)a, (Matrix *)b, context);
  }
  return result;
}

char Addition::operatorChar() const {
  return '+';
}

Expression * Addition::createEvaluationOnFloatAndMatrix(Float * a, Matrix * m, Context& context) const {
  Expression * operands[m->numberOfRows() * m->numberOfColumns()];
  for (int i = 0; i < m->numberOfRows() * m->numberOfColumns(); i++) {
    operands[i] = new Float(a->approximate(context) + m->operand(i)->approximate(context));
  }
  return new Matrix(operands, m->numberOfRows() * m->numberOfColumns(), m->numberOfColumns(), m->numberOfRows(), false);
}

Expression * Addition::createEvaluationOnMatrices(Matrix * m, Matrix * n, Context& context) const {
  if (m->numberOfColumns() != n->numberOfColumns() || m->numberOfRows() != n->numberOfColumns()) {
    return nullptr;
  }
  Expression * operands[m->numberOfRows() * m->numberOfColumns()];
  for (int i = 0; i < m->numberOfRows() * m->numberOfColumns(); i++) {
    operands[i] = new Float(m->operand(i)->approximate(context) + n->operand(i)->approximate(context));
  }
  return new Matrix(operands, m->numberOfRows() * m->numberOfColumns(), m->numberOfColumns(), m->numberOfRows(), false);
}
