#include <poincare/addition.h>
#include <poincare/complex_matrix.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include "layout/horizontal_layout.h"
#include "layout/string_layout.h"
#include "layout/parenthesis_layout.h"

namespace Poincare {

Expression::Type Addition::type() const {
  return Type::Addition;
}

ExpressionLayout * Addition::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout** children_layouts = new ExpressionLayout * [3];
  children_layouts[0] = m_operands[0]->createLayout(floatDisplayMode, complexFormat);
  children_layouts[1] = new StringLayout("+", 1);
  children_layouts[2] = m_operands[1]->type() == Type::Opposite ? new ParenthesisLayout(m_operands[1]->createLayout(floatDisplayMode, complexFormat)) : m_operands[1]->createLayout(floatDisplayMode, complexFormat);
  ExpressionLayout * layout = new HorizontalLayout(children_layouts, 3);
  delete[] children_layouts;
  return layout;
}

Complex Addition::compute(const Complex c, const Complex d) {
  return Complex::Cartesian(c.a()+d.a(), c.b()+d.b());
}

Evaluation * Addition::computeOnMatrices(Evaluation * m, Evaluation * n) {
  Addition a;
  return a.computeOnNumericalMatrices(m,n);
}

Evaluation * Addition::computeOnComplexAndMatrix(const Complex * c, Evaluation * m) {
  Addition a;
  return a.computeOnComplexAndComplexMatrix(c,m);
}

Expression * Addition::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  return new Addition(newOperands, cloneOperands);
}

bool Addition::isCommutative() const {
  return true;
}

}
