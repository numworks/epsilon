#include <poincare/determinant.h>
#include <poincare/matrix.h>
#include <poincare/evaluation.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type Determinant::type() const {
  return Type::Determinant;
}

Expression * Determinant::clone() const {
  Determinant * a = new Determinant(m_operands, true);
  return a;
}

template<typename T>
Evaluation<T> * Determinant::templatedEvaluate(Context& context, AngleUnit angleUnit) const {

  Evaluation<T> * input = operand(0)->evaluate<T>(context, angleUnit);
  Evaluation<T> * result = input->createDeterminant();
  delete input;
  return result;
}

}

