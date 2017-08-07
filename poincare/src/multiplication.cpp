extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>

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
  ExpressionLayout * children_layouts[3];
  children_layouts[0] = m_operands[0]->createLayout(floatDisplayMode, complexFormat);
  children_layouts[1] = new StringLayout("*", 1);
  children_layouts[2] = m_operands[1]->type() == Type::Opposite ? new ParenthesisLayout(m_operands[1]->createLayout(floatDisplayMode, complexFormat)) : m_operands[1]->createLayout(floatDisplayMode, complexFormat);
  return new HorizontalLayout(children_layouts, 3);
}

Expression * Multiplication::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 2);
  assert(newOperands != nullptr);
  return new Multiplication(newOperands, cloneOperands);
}

Complex Multiplication::compute(const Complex c, const Complex d) {
  return Complex::Cartesian(c.a()*d.a()-c.b()*d.b(), c.b()*d.a() + c.a()*d.b());
}

Evaluation * Multiplication::computeOnComplexAndMatrix(const Complex * c, Evaluation * m) {
  Multiplication mul;
  return mul.computeOnComplexAndComplexMatrix(c, m);
}

Evaluation * Multiplication::computeOnMatrices(Evaluation * m, Evaluation * n) {
  if (m->numberOfColumns() != n->numberOfRows()) {
    return new Complex(Complex::Float(NAN));
  }
  Complex * operands = new Complex[m->numberOfRows()*n->numberOfColumns()];
  for (int i = 0; i < m->numberOfRows(); i++) {
    for (int j = 0; j < n->numberOfColumns(); j++) {
      float a = 0.0f;
      float b = 0.0f;
      for (int k = 0; k < m->numberOfColumns(); k++) {
        Complex mEntry = *(m->complexOperand(i*m->numberOfColumns()+k));
        Complex nEntry = *(n->complexOperand(k*n->numberOfColumns()+j));
        a += mEntry.a()*nEntry.a() - mEntry.b()*nEntry.b();
        b += mEntry.b()*nEntry.a() + mEntry.a()*nEntry.b();
      }
      operands[i*n->numberOfColumns()+j] = Complex::Cartesian(a, b);
    }
  }
  Evaluation * result = new ComplexMatrix(operands, n->numberOfColumns(), m->numberOfRows());
  delete[] operands;
  return result;
}

}
