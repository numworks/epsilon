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

float Logarithm::approximate(Context& context, AngleUnit angleUnit) const {
  if (m_numberOfArguments == 1) {
    return log10f(m_args[0]->approximate(context, angleUnit));
  }
  return log10f(m_args[1]->approximate(context, angleUnit))/log10f(m_args[0]->approximate(context, angleUnit));
}

ExpressionLayout * Logarithm::createLayout(DisplayMode displayMode) const {
  if (m_numberOfArguments == 1) {
    return Function::createLayout(displayMode);
  }
  ExpressionLayout ** childrenLayouts = (ExpressionLayout **)malloc(2*sizeof(ExpressionLayout *));
  childrenLayouts[0] = new BaselineRelativeLayout(new StringLayout(m_name, strlen(m_name)), m_args[0]->createLayout(displayMode), BaselineRelativeLayout::Type::Subscript);
  childrenLayouts[1] = new ParenthesisLayout(m_args[1]->createLayout(displayMode));
  return new HorizontalLayout(childrenLayouts, 2);
}

}
