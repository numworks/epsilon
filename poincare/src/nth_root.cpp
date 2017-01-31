#include <poincare/nth_root.h>
#include "layout/nth_root_layout.h"

extern "C" {
#include <assert.h>
#include <math.h>
}

NthRoot::NthRoot() :
  Function("root")
{
}

Expression::Type NthRoot::type() const {
  return Type::NthRoot;
}

Expression * NthRoot::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  NthRoot * r = new NthRoot();
  r->setArgument(newOperands, numberOfOperands, cloneOperands);
  return r;
}

float NthRoot::approximate(Context& context) const {
  return powf(m_args[0]->approximate(context), 1.0f/m_args[1]->approximate(context));
}

ExpressionLayout * NthRoot::createLayout(DisplayMode displayMode) const {
  return new NthRootLayout(m_args[0]->createLayout(displayMode), m_args[1]->createLayout(displayMode));
}