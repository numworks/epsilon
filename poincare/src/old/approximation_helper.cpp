#include <float.h>
#include <omg/float.h>
#include <poincare/old/approximation_helper.h>
#include <poincare/old/evaluation.h>
#include <poincare/old/list_complex.h>
#include <poincare/old/matrix_complex.h>
#include <poincare/old/old_expression.h>
#include <stdint.h>

#include <cmath>
#include <utility>
extern "C" {
#include <assert.h>
}

namespace Poincare {

template <typename T>
bool ApproximationHelper::IsIntegerRepresentationAccurate(T x) {
  /* Float and double's precision to represent integers is limited by the size
   * of their mantissa. If an integer requires more digits than there is in the
   * mantissa, there will be a loss on precision that can be fatal on operations
   * such as GCD and LCM. */
  int digits = 0;
  // Compute number of digits (base 2) required to represent x
  std::frexp(x, &digits);
  // Compare it to the maximal number of digits that can be represented with <T>
  return digits <= (sizeof(T) == sizeof(double) ? DBL_MANT_DIG : FLT_MANT_DIG);
}

template <typename T>
uint32_t ApproximationHelper::PositiveIntegerApproximationIfPossible(
    const ExpressionNode *expression, bool *isUndefined,
    const ApproximationContext &approximationContext) {
  Evaluation<T> evaluation = expression->approximate(T(), approximationContext);
  T scalar = std::abs(evaluation.toScalar());
  /* Conversion from uint32 to float changes UINT32_MAX from 4294967295 to
   * 4294967296. */
  if (std::isnan(scalar) || scalar != std::round(scalar) ||
      scalar >= static_cast<T>(UINT32_MAX) ||
      !IsIntegerRepresentationAccurate(scalar)) {
    /* PositiveIntegerApproximationIfPossible returns undefined result if scalar
     * cannot be accurately represented as an unsigned integer. */
    *isUndefined = true;
    return 0;
  }
  return static_cast<uint32_t>(scalar);
}

/* Must be the maximum the number of parameters of functions that can distribute
 * over lists, currently hgeomcdfrange(m,q,N,K,n) */
constexpr static int k_maxNumberOfParametersForMap = 5;

template <typename T>
Evaluation<T> ApproximationHelper::Map(
    const ExpressionNode *expression,
    const ApproximationContext &approximationContext,
    ComplexesCompute<T> compute, BooleansCompute<T> booleansCompute,
    bool mapOnList, void *context) {
  Evaluation<T> evaluationArray[k_maxNumberOfParametersForMap];
  int numberOfParameters = expression->numberOfChildren();
  assert(numberOfParameters <= k_maxNumberOfParametersForMap);
  if (numberOfParameters == 0) {
    return Complex<T>::Undefined();
  }
  int listLength = OExpression::k_noList;
  for (int i = 0; i < numberOfParameters; i++) {
    evaluationArray[i] =
        expression->childAtIndex(i)->approximate(T(), approximationContext);
    if (evaluationArray[i].otype() == EvaluationNode<T>::Type::MatrixComplex ||
        evaluationArray[i].otype() ==
            EvaluationNode<T>::Type::PointEvaluation) {
      return Complex<T>::Undefined();
    }
    if (evaluationArray[i].otype() == EvaluationNode<T>::Type::ListComplex) {
      if (!mapOnList) {
        return Complex<T>::Undefined();
      }
      int newLength = evaluationArray[i].numberOfChildren();
      if (listLength == OExpression::k_noList) {
        listLength = newLength;
      } else if (listLength != newLength) {
        return Complex<T>::Undefined();
      }
      for (int j = 0; j < listLength; j++) {
        Evaluation<T> child = evaluationArray[i].childAtIndex(j);
        if (child.otype() == EvaluationNode<T>::Type::MatrixComplex ||
            child.otype() == EvaluationNode<T>::Type::PointEvaluation) {
          return Complex<T>::Undefined();
        }
      }
    }
  }

  std::complex<T> complexesArray[k_maxNumberOfParametersForMap];
  bool booleansArray[k_maxNumberOfParametersForMap];
  bool isBooleanEvaluation;
  if (evaluationArray[0].otype() == EvaluationNode<T>::Type::ListComplex &&
      evaluationArray[0].numberOfChildren() > 0) {
    isBooleanEvaluation = evaluationArray[0].childAtIndex(0).otype() ==
                          EvaluationNode<T>::Type::BooleanEvaluation;
  } else {
    isBooleanEvaluation = evaluationArray[0].otype() ==
                          EvaluationNode<T>::Type::BooleanEvaluation;
  }
  if (listLength == OExpression::k_noList) {
    for (int i = 0; i < numberOfParameters; i++) {
      assert(evaluationArray[i].otype() == EvaluationNode<T>::Type::Complex ||
             evaluationArray[i].otype() ==
                 EvaluationNode<T>::Type::BooleanEvaluation);
      if ((evaluationArray[i].otype() ==
           EvaluationNode<T>::Type::BooleanEvaluation) != isBooleanEvaluation) {
        return Complex<T>::Undefined();
      }
      if (isBooleanEvaluation) {
        booleansArray[i] =
            static_cast<BooleanEvaluation<T> &>(evaluationArray[i]).value();
      } else {
        complexesArray[i] = evaluationArray[i].complexAtIndex(0);
      }
    }
    return isBooleanEvaluation
               ? booleansCompute(booleansArray, numberOfParameters, context)
               : Complex<T>::Builder(
                     compute(complexesArray, numberOfParameters,
                             approximationContext.complexFormat(),
                             approximationContext.angleUnit(), context));
  }
  ListComplex<T> resultList = ListComplex<T>::Builder();
  for (int k = 0; k < listLength; k++) {
    for (int i = 0; i < numberOfParameters; i++) {
      Evaluation<T> currentChild;
      if (evaluationArray[i].otype() == EvaluationNode<T>::Type::ListComplex) {
        currentChild = evaluationArray[i].childAtIndex(k);
      } else {
        currentChild = evaluationArray[i];
      }
      if ((currentChild.otype() ==
           EvaluationNode<T>::Type::BooleanEvaluation) != isBooleanEvaluation) {
        return Complex<T>::Undefined();
      }
      if (currentChild.otype() == EvaluationNode<T>::Type::Complex) {
        complexesArray[i] = currentChild.complexAtIndex(0);
      } else {
        assert(currentChild.otype() ==
               EvaluationNode<T>::Type::BooleanEvaluation);
        booleansArray[i] =
            static_cast<BooleanEvaluation<T> &>(currentChild).value();
      }
    }
    if (isBooleanEvaluation) {
      resultList.addChildAtIndexInPlace(
          booleansCompute(booleansArray, numberOfParameters, context), k, k);
    } else {
      resultList.addChildAtIndexInPlace(
          Complex<T>::Builder(compute(complexesArray, numberOfParameters,
                                      approximationContext.complexFormat(),
                                      approximationContext.angleUnit(),
                                      context)),
          k, k);
    }
  }
  return std::move(resultList);
}

template bool
Poincare::ApproximationHelper::IsIntegerRepresentationAccurate<float>(float x);
template bool Poincare::ApproximationHelper::IsIntegerRepresentationAccurate<
    double>(double x);

template uint32_t
Poincare::ApproximationHelper::PositiveIntegerApproximationIfPossible<float>(
    Poincare::ExpressionNode const *, bool *, const ApproximationContext &);
template uint32_t
Poincare::ApproximationHelper::PositiveIntegerApproximationIfPossible<double>(
    Poincare::ExpressionNode const *, bool *, const ApproximationContext &);

template Poincare::Evaluation<float> Poincare::ApproximationHelper::Map(
    const Poincare::ExpressionNode *expression, const ApproximationContext &,
    Poincare::ApproximationHelper::ComplexesCompute<float> compute,
    Poincare::ApproximationHelper::BooleansCompute<float> booleansCompute,
    bool mapOnList, void *context);
template Poincare::Evaluation<double> Poincare::ApproximationHelper::Map(
    const Poincare::ExpressionNode *expression, const ApproximationContext &,
    Poincare::ApproximationHelper::ComplexesCompute<double> compute,
    Poincare::ApproximationHelper::BooleansCompute<double> booleansCompute,
    bool mapOnList, void *context);

}  // namespace Poincare
