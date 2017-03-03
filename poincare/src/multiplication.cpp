extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <math.h>
}

#include <poincare/multiplication.h>
#include "layout/string_layout.h"
#include "layout/horizontal_layout.h"
#include "layout/parenthesis_layout.h"

namespace Poincare {

Expression::Type Multiplication::type() const {
  return Expression::Type::Multiplication;
}

ExpressionLayout * Multiplication::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout** children_layouts = (ExpressionLayout **)malloc(3*sizeof(ExpressionLayout *));
  children_layouts[0] = m_operands[0]->createLayout(floatDisplayMode, complexFormat);
  children_layouts[1] = new StringLayout("*", 1);
  children_layouts[2] = m_operands[1]->type() == Type::Opposite ? new ParenthesisLayout(m_operands[1]->createLayout(floatDisplayMode, complexFormat)) : m_operands[1]->createLayout(floatDisplayMode, complexFormat);
  return new HorizontalLayout(children_layouts, 3);
}

float Multiplication::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  return m_operands[0]->approximate(context, angleUnit)*m_operands[1]->approximate(context, angleUnit);;
}

Expression * Multiplication::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 2);
  assert(newOperands != nullptr);
  return new Multiplication(newOperands, cloneOperands);
}

Expression * Multiplication::evaluateOnComplex(Complex * c, Complex * d, Context& context, AngleUnit angleUnit) const {
  return new Complex(Complex::Cartesian(c->a()*d->a()-c->b()*d->b(), c->b()*d->a() + c->a()*d->b()));
}

Expression * Multiplication::evaluateOnMatrices(Matrix * m, Matrix * n, Context& context, AngleUnit angleUnit) const {
  if (m->numberOfColumns() != n->numberOfRows()) {
    return nullptr;
  }
  Expression * operands[m->numberOfRows() * n->numberOfColumns()];
  for (int i = 0; i < m->numberOfRows(); i++) {
    for (int j = 0; j < n->numberOfColumns(); j++) {
      float a = 0.0f;
      float b = 0.0f;
      for (int k = 0; k < m->numberOfColumns(); k++) {
        Expression * mEvaluation = m->operand(i*m->numberOfColumns()+k)->evaluate(context, angleUnit);
        Expression * nEvaluation = n->operand(k*n->numberOfColumns()+j)->evaluate(context, angleUnit);
        assert(mEvaluation->type() == Type::Matrix || mEvaluation->type() == Type::Complex);
        assert(nEvaluation->type() == Type::Matrix || nEvaluation->type() == Type::Complex);
        if (mEvaluation->type() == Type::Matrix ||nEvaluation->type() == Type::Matrix) {
          operands[i] = new Complex(Complex::Float(NAN));
          delete mEvaluation;
          delete nEvaluation;
          continue;
        }
        a += ((Complex *)mEvaluation)->a()*((Complex *)nEvaluation)->a() - ((Complex *)mEvaluation)->b()*((Complex *)nEvaluation)->b();
        b += ((Complex *)mEvaluation)->b()*((Complex *)nEvaluation)->a() + ((Complex *)mEvaluation)->a()*((Complex *)nEvaluation)->b();
        delete mEvaluation;
        delete nEvaluation;
      }
      operands[i*n->numberOfColumns()+j] = new Complex(Complex::Cartesian(a, b));
    }
  }
  return new Matrix(operands, m->numberOfRows() * n->numberOfColumns(), m->numberOfRows(), n->numberOfColumns(), false);
}

}
