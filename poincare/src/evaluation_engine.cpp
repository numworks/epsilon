#include <poincare/evaluation_engine.h>
#include <poincare/matrix.h>
#include <cmath>
extern "C" {
#include <assert.h>
}

namespace Poincare {

template<typename T> Complex<T> * EvaluationEngine::approximate(const Expression * expression, Context& context, Expression::AngleUnit angleUnit, ComplexCompute<T> compute) {
  assert(expression->numberOfOperands() == 1);
  Complex<T> * input = expression->operand(0)->privateEvaluate(T(), context, angleUnit);
  Complex<T> * result = new Complex<T>(compute(*input, angleUnit));
  delete input;
  return result;
}

template<typename T> Complex<T> * EvaluationEngine::mapReduce(const Expression * expression, Context& context, Expression::AngleUnit angleUnit, ComplexAndComplexReduction<T> computeOnComplexes) {
  Complex<T> * result = expression->operand(0)->privateEvaluate(T(), context, angleUnit);
  for (int i = 1; i < expression->numberOfOperands(); i++) {
    Complex<T> * nextOperandEvaluation = expression->operand(i)->privateEvaluate(T(), context, angleUnit);
    Complex<T> * intermediateResult = new Complex<T>(computeOnComplexes(*result, *nextOperandEvaluation));
    delete result;
    delete nextOperandEvaluation;
    result = intermediateResult;
    if (result == nullptr) {
      return new Complex<T>(Complex<T>::Float(NAN));
    }
  }
  return result;
}

template Complex<float> * Poincare::EvaluationEngine::approximate(const Expression * expression, Context& context, Expression::AngleUnit angleUnit, EvaluationEngine::ComplexCompute<float> compute);
template Complex<double> * EvaluationEngine::approximate(const Expression * expression, Context& context, Expression::AngleUnit angleUnit, EvaluationEngine::ComplexCompute<double> compute);
template Complex<float> * EvaluationEngine::mapReduce(const Expression * expression, Context& context, Expression::AngleUnit angleUnit, EvaluationEngine::ComplexAndComplexReduction<float> computeOnComplexes);
template Complex<double> * EvaluationEngine::mapReduce(const Expression * expression, Context& context, Expression::AngleUnit angleUnit, EvaluationEngine::ComplexAndComplexReduction<double> computeOnComplexes);

}
