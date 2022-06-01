#include <poincare/approximation_helper.h>
#include <poincare/expression.h>
#include <poincare/evaluation.h>
#include <poincare/float.h>
#include <poincare/list_complex.h>
#include <poincare/matrix_complex.h>
#include <cmath>
#include <float.h>
#include <stdint.h>
#include <utility>
extern "C" {
#include <assert.h>
}

namespace Poincare {

template <typename T> bool isNegligeable(T x, T precision, T norm1, T norm2) {
  T absX = std::fabs(x);
  return absX <= 10.0*precision && absX/norm1 <= precision && absX/norm2 <= precision;
}

template < typename T> T minimalNonNullMagnitudeOfParts(std::complex<T> c) {
  T absRealInput = std::fabs(c.real());
  T absImagInput = std::fabs(c.imag());
  // If the magnitude of one part is null, ignore it
  if (absRealInput == static_cast<T>(0.0) || (absImagInput > static_cast<T>(0.0) && absImagInput < absRealInput)) {
    return absImagInput;
  }
  return absRealInput;
}

template<typename T>
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

template <typename T> uint32_t ApproximationHelper::PositiveIntegerApproximationIfPossible(const ExpressionNode * expression, bool * isUndefined, ExpressionNode::ApproximationContext approximationContext) {
  Evaluation<T> evaluation = expression->approximate(T(), approximationContext);
  T scalar = std::abs(evaluation.toScalar());
  if (std::isnan(scalar) || scalar != std::round(scalar) || scalar > UINT32_MAX || !IsIntegerRepresentationAccurate(scalar)) {
    /* PositiveIntegerApproximationIfPossible returns undefined result if scalar
     * cannot be accurately represented as an unsigned integer. */
    *isUndefined = true;
    return 0;
  }
  return static_cast<uint32_t>(scalar);
}

template <typename T> std::complex<T> ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(std::complex<T> result, std::complex<T> input1, std::complex<T> input2, bool enableNullResult) {
  /* Cheat: openbsd  functions (cos, sin, tan, cosh, acos, pow...) are
   * numerical implementation and thus are approximative.
   * The error epsilon is ~1E-7 on float and ~1E-15 on double. In order to avoid
   * weird results as acos(1) = 6E-17 or cos(π/2) = 4E-17, we round the result
   * to its 1E-6 or 1E-14 precision when its ratio with the argument (π/2 in the
   * example) is smaller than epsilon. This way, we have sin(π) ~ 0 and
   * sin(1E-15)=1E-15.
   * We can't do that for all evaluation as the user can operate on values as
   * small as 1E-308 (in double) and most results still be correct. */
  if (!enableNullResult && (result.imag() == 0.0 || result.real() == 0.0)) {
    return result;
  }
  T magnitude1 = minimalNonNullMagnitudeOfParts(input1);
  T magnitude2 = minimalNonNullMagnitudeOfParts(input2);
  T precision = Float<T>::EpsilonLax();
  if (isNegligeable(result.imag(), precision, magnitude1, magnitude2)) {
    result.imag(0);
  }
  if (isNegligeable(result.real(), precision, magnitude1, magnitude2)) {
    result.real(0);
  }
  return result;
}

template<typename T> ListComplex<T> ElementWiseOnListAndComplex(const ListComplex<T> l, const std::complex<T> c, Preferences::ComplexFormat complexFormat, ApproximationHelper::ComplexAndComplexReduction<T> computeOnComplexes, bool complexFirst) {
  if (l.isUndefined()) {
    return ListComplex<T>::Undefined();
  }
  ListComplex<T> result = ListComplex<T>::Builder();
  int nChildren = l.numberOfChildren();
  for (int i = 0; i < nChildren; i++) {
    std::complex<T> complexAtIndex = l.complexAtIndex(i);
    Evaluation<T> computedNewComplex;
    if (complexFirst) {
      computedNewComplex = computeOnComplexes(c, complexAtIndex, complexFormat);
    } else {
      computedNewComplex = computeOnComplexes(complexAtIndex, c, complexFormat);
    }
    result.addChildAtIndexInPlace(computedNewComplex, i, i);
  }
  return result;
}

template<typename T> ListComplex<T> ElementWiseOnLists(const ListComplex<T> l1, const ListComplex<T> l2, Preferences::ComplexFormat complexFormat, ApproximationHelper::ComplexAndComplexReduction<T> computeOnComplexes) {
  if (l1.isUndefined() || l2.isUndefined() || l1.numberOfChildren() != l2.numberOfChildren()) {
    return ListComplex<T>::Undefined();
  }
  ListComplex<T> result = ListComplex<T>::Builder();
  int nChildren = l1.numberOfChildren();
  for (int i = 0; i < nChildren; i++) {
    result.addChildAtIndexInPlace(computeOnComplexes(l1.complexAtIndex(i), l2.complexAtIndex(i), complexFormat), i, i);
  }
  return result;
}

constexpr static int k_maxNumberOfParametersForMap = 4;

template<typename T> Evaluation<T> ApproximationHelper::Map(const ExpressionNode * expression, ExpressionNode::ApproximationContext approximationContext, ComplexesCompute<T> compute, bool mapOnList, void * context) {

  Evaluation<T> evaluationArray[k_maxNumberOfParametersForMap];
  int numberOfParameters = expression->numberOfChildren();
  assert(numberOfParameters <= k_maxNumberOfParametersForMap);
  if (numberOfParameters == 0) {
    return Complex<T>::Undefined();
  }
  int listLength = Expression::k_noList;
  for (int i = 0; i < numberOfParameters; i++) {
    evaluationArray[i] = expression->childAtIndex(i)->approximate(T(), approximationContext);
    if (evaluationArray[i].type() == EvaluationNode<T>::Type::MatrixComplex) {
      return Complex<T>::Undefined();
    }
    if (evaluationArray[i].type() == EvaluationNode<T>::Type::ListComplex) {
      if (!mapOnList) {
        return Complex<T>::Undefined();
      }
      int newLength = evaluationArray[i].numberOfChildren();
      if (listLength == Expression::k_noList) {
        listLength = newLength;
      } else if (listLength != newLength) {
        return Complex<T>::Undefined();
      }
    }
  }

  std::complex<T> complexesArray[k_maxNumberOfParametersForMap];
  if (listLength == Expression::k_noList) {
    for (int i = 0; i < numberOfParameters; i++) {
      assert(evaluationArray[i].type() == EvaluationNode<T>::Type::Complex);
      complexesArray[i] = evaluationArray[i].complexAtIndex(0);
    }
    return compute(complexesArray, numberOfParameters, approximationContext.complexFormat(), approximationContext.angleUnit(), context);
  }
  ListComplex<T> resultList = ListComplex<T>::Builder();
  for (int k = 0; k < listLength; k++) {
    for (int i = 0; i < numberOfParameters; i++) {
      if (evaluationArray[i].type() == EvaluationNode<T>::Type::Complex) {
        complexesArray[i] = evaluationArray[i].complexAtIndex(0);
      } else {
        assert(evaluationArray[i].type() == EvaluationNode<T>::Type::ListComplex);
        complexesArray[i] = evaluationArray[i].complexAtIndex(k);
      }
    }
    resultList.addChildAtIndexInPlace(compute(complexesArray, numberOfParameters, approximationContext.complexFormat(), approximationContext.angleUnit(), context), k, k);
  }
  return std::move(resultList);
}

template<typename T> Evaluation<T> ApproximationHelper::MapOneChild(const ExpressionNode * expression, ExpressionNode::ApproximationContext approximationContext, ComplexCompute<T> compute, bool mapOnList) {
  assert(expression->numberOfChildren() == 1);
  return Map<T>(expression,
      approximationContext,
      [] (const std::complex<T> * c, int numberOfComplexes, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, void * context) {
        assert(numberOfComplexes == 1);
        return reinterpret_cast<ComplexCompute<T>>(context)(c[0], complexFormat, angleUnit);
      },
      mapOnList,
      reinterpret_cast<void *>(compute));
}

template<typename T> Evaluation<T> ApproximationHelper::Reduce(
    Evaluation<T> eval1,
    Evaluation<T> eval2,
    Preferences::ComplexFormat complexFormat,
    ComplexAndComplexReduction<T> computeOnComplexes,
    ComplexAndMatrixReduction<T> computeOnComplexAndMatrix,
    MatrixAndComplexReduction<T> computeOnMatrixAndComplex,
    MatrixAndMatrixReduction<T> computeOnMatrices,
    bool mapOnList
    ) {
  // If element is complex
  if (eval1.type() == EvaluationNode<T>::Type::Complex) {
    if (eval2.type() == EvaluationNode<T>::Type::Complex) {
       return computeOnComplexes(eval1.complexAtIndex(0), eval2.complexAtIndex(0), complexFormat);
    } else if (eval2.type() == EvaluationNode<T>::Type::ListComplex) {
      if (!mapOnList) {
        return Complex<T>::Undefined();
      }
      return ElementWiseOnListAndComplex<T>(static_cast<ListComplex<T> &>(eval2), eval1.complexAtIndex(0), complexFormat, computeOnComplexes, true);
    } else {
      assert(eval2.type() == EvaluationNode<T>::Type::MatrixComplex);
      return computeOnComplexAndMatrix(eval1.complexAtIndex(0), static_cast<MatrixComplex<T> &>(eval2), complexFormat);
    }
  // If element is list
  } else if (eval1.type() == EvaluationNode<T>::Type::ListComplex) {
    if (!mapOnList) {
      return Complex<T>::Undefined();
    }
    if (eval2.type() == EvaluationNode<T>::Type::Complex) {
       return ElementWiseOnListAndComplex<T>(static_cast<ListComplex<T> &>(eval1), eval2.complexAtIndex(0), complexFormat, computeOnComplexes, false);
    } else if (eval2.type() == EvaluationNode<T>::Type::ListComplex) {
      return ElementWiseOnLists<T>(static_cast<ListComplex<T> &>(eval1), static_cast<ListComplex<T> &>(eval2), complexFormat, computeOnComplexes);
    } else {
      // Matrices and lists are not compatible
      assert(eval2.type() == EvaluationNode<T>::Type::MatrixComplex);
      return Complex<T>::Undefined();
    }
  // If element if matrix
  } else {
    assert(eval1.type() == EvaluationNode<T>::Type::MatrixComplex);
    if (eval2.type() == EvaluationNode<T>::Type::Complex) {
       return computeOnMatrixAndComplex(static_cast<MatrixComplex<T> &>(eval1), eval2.complexAtIndex(0), complexFormat);
    } else if (eval2.type() == EvaluationNode<T>::Type::MatrixComplex) {
       return computeOnMatrices(static_cast<MatrixComplex<T> &>(eval1), static_cast<MatrixComplex<T> &>(eval2), complexFormat);
    } else {
      // Matrices and lists are not compatible
      assert(eval2.type() == EvaluationNode<T>::Type::ListComplex);
      return Complex<T>::Undefined();
    }
  }
}

template<typename T> Evaluation<T> ApproximationHelper::MapReduce(
    const ExpressionNode * expression,
    ExpressionNode::ApproximationContext approximationContext,
    ReductionFunction<T> reductionFunction
    ) {
  assert(expression->numberOfChildren() > 0);
  Evaluation<T> result = expression->childAtIndex(0)->approximate(T(), approximationContext);
  for (int i = 1; i < expression->numberOfChildren(); i++) {
    Evaluation<T> nextOperandEvaluation = expression->childAtIndex(i)->approximate(T(), approximationContext);
    result = reductionFunction(result, nextOperandEvaluation, approximationContext.complexFormat());
    if (result.isUndefined()) {
      return Complex<T>::Undefined();
    }
  }
  return result;
}

template<typename T> MatrixComplex<T> ApproximationHelper::ElementWiseOnMatrixAndComplex(const MatrixComplex<T> m, const std::complex<T> c, Poincare::Preferences::ComplexFormat complexFormat, ComplexAndComplexReduction<T> computeOnComplexes) {
  MatrixComplex<T> matrix = MatrixComplex<T>::Builder();
  for (int i = 0; i < m.numberOfChildren(); i++) {
    matrix.addChildAtIndexInPlace(computeOnComplexes(m.complexAtIndex(i), c, complexFormat), i, i);
  }
  matrix.setDimensions(m.numberOfRows(), m.numberOfColumns());
  return matrix;
}

template<typename T> MatrixComplex<T> ApproximationHelper::ElementWiseOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, Poincare::Preferences::ComplexFormat complexFormat, ComplexAndComplexReduction<T> computeOnComplexes) {
  if (m.numberOfRows() != n.numberOfRows() || m.numberOfColumns() != n.numberOfColumns()) {
    return MatrixComplex<T>::Undefined();
  }
  MatrixComplex<T> matrix = MatrixComplex<T>::Builder();
  for (int i = 0; i < m.numberOfChildren(); i++) {
    matrix.addChildAtIndexInPlace(computeOnComplexes(m.complexAtIndex(i), n.complexAtIndex(i), complexFormat), i, i);
  }
  matrix.setDimensions(m.numberOfRows(), m.numberOfColumns());
  return matrix;
}

template bool Poincare::ApproximationHelper::IsIntegerRepresentationAccurate<float>(float x);
template bool Poincare::ApproximationHelper::IsIntegerRepresentationAccurate<double>(double x);

template uint32_t Poincare::ApproximationHelper::PositiveIntegerApproximationIfPossible<float>(Poincare::ExpressionNode const*, bool*, ExpressionNode::ApproximationContext);
template uint32_t Poincare::ApproximationHelper::PositiveIntegerApproximationIfPossible<double>(Poincare::ExpressionNode const*, bool*, ExpressionNode::ApproximationContext);

template std::complex<float> Poincare::ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable<float>(std::complex<float>,std::complex<float>,std::complex<float>,bool);
template std::complex<double> Poincare::ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable<double>(std::complex<double>,std::complex<double>,std::complex<double>,bool);

template Poincare::Evaluation<float> Poincare::ApproximationHelper::Map(const Poincare::ExpressionNode * expression, ExpressionNode::ApproximationContext, Poincare::ApproximationHelper::ComplexesCompute<float> compute, bool mapOnList, void * context);
template Poincare::Evaluation<double> Poincare::ApproximationHelper::Map(const Poincare::ExpressionNode * expression, ExpressionNode::ApproximationContext, Poincare::ApproximationHelper::ComplexesCompute<double> compute, bool mapOnList, void * context);

template Poincare::Evaluation<float> Poincare::ApproximationHelper::MapOneChild(const Poincare::ExpressionNode * expression, ExpressionNode::ApproximationContext, Poincare::ApproximationHelper::ComplexCompute<float> compute, bool mapOnList);
template Poincare::Evaluation<double> Poincare::ApproximationHelper::MapOneChild(const Poincare::ExpressionNode * expression, ExpressionNode::ApproximationContext, Poincare::ApproximationHelper::ComplexCompute<double> compute, bool mapOnList);

template Poincare::Evaluation<float> Poincare::ApproximationHelper::MapReduce<float>(const Poincare::ExpressionNode * expression, Poincare::ExpressionNode::ApproximationContext approximationContext, Poincare::ApproximationHelper::ReductionFunction<float> reductionFunction);
template Poincare::Evaluation<double> Poincare::ApproximationHelper::MapReduce<double>(const Poincare::ExpressionNode * expression, Poincare::ExpressionNode::ApproximationContext approximationContext, Poincare::ApproximationHelper::ReductionFunction<double> reductionFunction);

template Poincare::MatrixComplex<float> Poincare::ApproximationHelper::ElementWiseOnMatrixAndComplex<float>(const Poincare::MatrixComplex<float>, const std::complex<float>, Poincare::Preferences::ComplexFormat, Poincare::Complex<float> (*)(std::complex<float>, std::complex<float>, Poincare::Preferences::ComplexFormat));
template Poincare::MatrixComplex<double> Poincare::ApproximationHelper::ElementWiseOnMatrixAndComplex<double>(const Poincare::MatrixComplex<double>, std::complex<double> const, Poincare::Preferences::ComplexFormat, Poincare::Complex<double> (*)(std::complex<double>, std::complex<double>, Poincare::Preferences::ComplexFormat));

template Poincare::MatrixComplex<float> Poincare::ApproximationHelper::ElementWiseOnMatrices<float>(const Poincare::MatrixComplex<float>, const Poincare::MatrixComplex<float>, Poincare::Preferences::ComplexFormat, Poincare::Complex<float> (*)(std::complex<float>, std::complex<float>, Poincare::Preferences::ComplexFormat));
template Poincare::MatrixComplex<double> Poincare::ApproximationHelper::ElementWiseOnMatrices<double>(const Poincare::MatrixComplex<double>, const Poincare::MatrixComplex<double>, Poincare::Preferences::ComplexFormat, Poincare::Complex<double> (*)(std::complex<double>, std::complex<double>, Poincare::Preferences::ComplexFormat));

template Poincare::Evaluation<float> Poincare::ApproximationHelper::Reduce(Poincare::Evaluation<float> eval1, Poincare::Evaluation<float> eval2, Poincare::Preferences::ComplexFormat complexFormat, Poincare::ApproximationHelper::ComplexAndComplexReduction<float> computeOnComplexes, Poincare::ApproximationHelper::ComplexAndMatrixReduction<float> computeOnComplexAndMatrix, Poincare::ApproximationHelper::MatrixAndComplexReduction<float> computeOnMatrixAndComplex, Poincare::ApproximationHelper::MatrixAndMatrixReduction<float> computeOnMatrices, bool mapOnList);
template Poincare::Evaluation<double> Poincare::ApproximationHelper::Reduce(Poincare::Evaluation<double> eval1, Poincare::Evaluation<double> eval2, Poincare::Preferences::ComplexFormat complexFormat, Poincare::ApproximationHelper::ComplexAndComplexReduction<double> computeOnComplexes, Poincare::ApproximationHelper::ComplexAndMatrixReduction<double> computeOnComplexAndMatrix, Poincare::ApproximationHelper::MatrixAndComplexReduction<double> computeOnMatrixAndComplex, Poincare::ApproximationHelper::MatrixAndMatrixReduction<double> computeOnMatrices, bool mapOnList);

}
