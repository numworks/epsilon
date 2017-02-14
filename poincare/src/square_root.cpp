#include <poincare/square_root.h>
#include <poincare/complex.h>
#include <poincare/power.h>
#include "layout/nth_root_layout.h"
extern "C" {
#include <assert.h>
#include <math.h>
}

SquareRoot::SquareRoot() :
  Function("v")
{
}

Expression::Type SquareRoot::type() const {
  return Type::SquareRoot;
}

Expression * SquareRoot::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  SquareRoot * sr = new SquareRoot();
  sr->setArgument(newOperands, numberOfOperands, cloneOperands);
  return sr;
}

float SquareRoot::approximate(Context& context, AngleUnit angleUnit) const {
  return powf(m_args[0]->approximate(context, angleUnit), 1.0f/2.0f);
}

ExpressionLayout * SquareRoot::createLayout(DisplayMode displayMode) const {
  return new NthRootLayout(m_args[0]->createLayout(displayMode),nullptr);
}

Expression * SquareRoot::evaluate(Context& context, AngleUnit angleUnit) const {
  Expression * evaluation = m_args[0]->evaluate(context, angleUnit);
  assert(evaluation->type() == Type::Matrix || evaluation->type() == Type::Complex);
  if (evaluation->type() == Type::Matrix) {
    delete evaluation;
    return new Complex(NAN);
  }
  Expression * operands[2];
  operands[0] = evaluation;
  operands[1] = new Complex(0.5f);
  Expression * power = new Power(operands, true);
  Expression * newResult = power->evaluate(context, angleUnit);
  delete evaluation;
  delete operands[1];
  delete power;
  return newResult;
}
