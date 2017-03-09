#include <poincare/addition.h>
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
  ExpressionLayout** children_layouts = (ExpressionLayout **)malloc(3*sizeof(ExpressionLayout *));
  children_layouts[0] = m_operands[0]->createLayout(floatDisplayMode, complexFormat);
  children_layouts[1] = new StringLayout("+", 1);
  children_layouts[2] = m_operands[1]->type() == Type::Opposite ? new ParenthesisLayout(m_operands[1]->createLayout(floatDisplayMode, complexFormat)) : m_operands[1]->createLayout(floatDisplayMode, complexFormat);
  ExpressionLayout * layout = new HorizontalLayout(children_layouts, 3);
  free(children_layouts);
  return layout;
}

float Addition::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  return m_operands[0]->approximate(context, angleUnit)+m_operands[1]->approximate(context, angleUnit);;
}

Expression * Addition::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  return new Addition(newOperands, cloneOperands);
}

bool Addition::isCommutative() const {
  return true;
}

Expression * Addition::evaluateOnComplex(Complex * c, Complex * d, Context& context, AngleUnit angleUnit) const {
  return new Complex(Complex::Cartesian(c->a()+ d->a(), c->b() + d->b()));
}

}
