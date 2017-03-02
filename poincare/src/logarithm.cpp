#include <poincare/logarithm.h>
extern "C" {
#include <assert.h>
#include <math.h>
#include <stdlib.h>
}
#include "layout/horizontal_layout.h"
#include "layout/parenthesis_layout.h"
#include "layout/string_layout.h"
#include "layout/baseline_relative_layout.h"

namespace Poincare {

Logarithm::Logarithm() :
  Function("log")
{
}

Expression::Type Logarithm::type() const {
  return Type::Logarithm;
}

Expression * Logarithm::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1 || numberOfOperands == 2);
  assert(newOperands != nullptr);
  Logarithm * l = new Logarithm();
  l->setArgument(newOperands, numberOfOperands, cloneOperands);
  return l;
}

float Logarithm::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  if (m_numberOfArguments == 1) {
    return log10f(m_args[0]->approximate(context, angleUnit));
  }
  return log10f(m_args[1]->approximate(context, angleUnit))/log10f(m_args[0]->approximate(context, angleUnit));
}

ExpressionLayout * Logarithm::privateCreateLayout(FloatDisplayMode floatDisplayMode) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  if (m_numberOfArguments == 1) {
    return Function::privateCreateLayout(floatDisplayMode);
  }
  ExpressionLayout ** childrenLayouts = (ExpressionLayout **)malloc(2*sizeof(ExpressionLayout *));
  childrenLayouts[0] = new BaselineRelativeLayout(new StringLayout(m_name, strlen(m_name)), m_args[0]->createLayout(floatDisplayMode), BaselineRelativeLayout::Type::Subscript);
  childrenLayouts[1] = new ParenthesisLayout(m_args[1]->createLayout(floatDisplayMode));
  return new HorizontalLayout(childrenLayouts, 2);
}

}
