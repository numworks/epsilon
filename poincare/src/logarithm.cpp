#include <poincare/logarithm.h>
#include <poincare/fraction.h>
#include <cmath>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include "layout/baseline_relative_layout.h"
#include "layout/horizontal_layout.h"
#include "layout/parenthesis_layout.h"
#include "layout/string_layout.h"

namespace Poincare {

Expression::Type Logarithm::type() const {
  return Type::Logarithm;
}

Expression * Logarithm::clone() const {
  return new Logarithm(operands(), true);
}

bool Logarithm::isCommutative() const {
  return false;
}

template<typename T>
Complex<T> Logarithm::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  return Complex<T>::Float(std::log10(c.a()));
}

template<typename T>
Evaluation<T> * Logarithm::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  if (numberOfOperands() == 1) {
    return EvaluationEngine::map(this, context, angleUnit, computeOnComplex<T>);
  }
  Evaluation<T> * x = operand(0)->evaluate<T>(context, angleUnit);
  Evaluation<T> * n = operand(1)->evaluate<T>(context, angleUnit);
  if (x->numberOfRows() != 1 || x->numberOfColumns() != 1 || n->numberOfRows() != 1 || n->numberOfColumns() != 1) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  Complex<T> result = Fraction::compute<T>(computeOnComplex(*(n->complexOperand(0)), angleUnit), computeOnComplex(*(x->complexOperand(0)), angleUnit));
  delete x;
  delete n;
  return new Complex<T>(result);
}

ExpressionLayout * Logarithm::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  if (numberOfOperands() == 1) {
    return LayoutEngine::createPrefixLayout(this, floatDisplayMode, complexFormat, "log");
  }
  ExpressionLayout * childrenLayouts[2];
  childrenLayouts[0] = new BaselineRelativeLayout(new StringLayout("log", strlen("log")), operand(0)->createLayout(floatDisplayMode, complexFormat), BaselineRelativeLayout::Type::Subscript);
  childrenLayouts[1] = new ParenthesisLayout(operand(1)->createLayout(floatDisplayMode, complexFormat));
  return new HorizontalLayout(childrenLayouts, 2);
}

}
