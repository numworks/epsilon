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

template<typename T> Evaluation<T> ApproximationHelper::Map(const ExpressionNode * expression, ExpressionNode::ApproximationContext approximationContext, ComplexCompute<T> compute, bool mapOnMatrix, bool mapOnList) {
  assert(expression->numberOfChildren() == 1);
  Evaluation<T> input = expression->childAtIndex(0)->approximate(T(), approximationContext);
  if (input.type() == EvaluationNode<T>::Type::Complex) {
    return compute(static_cast<Complex<T> &>(input).stdComplex(), approximationContext.complexFormat(), approximationContext.angleUnit());
  } else if (input.type() == EvaluationNode<T>::Type::ListComplex) {
     if (!mapOnList) {
      return Complex<T>::Undefined();
    }
    ListComplex<T> list = static_cast<ListComplex<T> &>(input);
    ListComplex<T> result = ListComplex<T>::Builder();
    for (int i = 0; i < list.numberOfChildren(); i++) {
      result.addChildAtIndexInPlace(compute(list.complexAtIndex(i), approximationContext.complexFormat(), approximationContext.angleUnit()), i, i);
    }
    return std::move(result);
  } else {
    assert(input.type() == EvaluationNode<T>::Type::MatrixComplex);
    if (!mapOnMatrix) {
      return Complex<T>::Undefined();
    }
    MatrixComplex<T> m = static_cast<MatrixComplex<T> &>(input);
    MatrixComplex<T> result = MatrixComplex<T>::Builder();
    for (int i = 0; i < m.numberOfChildren(); i++) {
      result.addChildAtIndexInPlace(compute(m.complexAtIndex(i), approximationContext.complexFormat(), approximationContext.angleUnit()), i, i);
    }
    result.setDimensions(m.numberOfRows(), m.numberOfColumns());
    return std::move(result);
  }
}

template<typename T> Evaluation<T> ApproximationHelper::MapReduce(
    const ExpressionNode * expression,
    ExpressionNode::ApproximationContext approximationContext,
    ComplexAndComplexReduction<T> computeOnComplexes,
    ComplexAndMatrixReduction<T> computeOnComplexAndMatrix,
    MatrixAndComplexReduction<T> computeOnMatrixAndComplex,
    MatrixAndMatrixReduction<T> computeOnMatrices,
    bool mapOnList
    ) {
  assert(expression->numberOfChildren() > 0);
  Evaluation<T> result = expression->childAtIndex(0)->approximate(T(), approximationContext);
  for (int i = 1; i < expression->numberOfChildren(); i++) {
    Evaluation<T> intermediateResult;
    Evaluation<T> nextOperandEvaluation = expression->childAtIndex(i)->approximate(T(), approximationContext);
    // If element is complex
    if (result.type() == EvaluationNode<T>::Type::Complex) {
      if (nextOperandEvaluation.type() == EvaluationNode<T>::Type::Complex) {
        intermediateResult = computeOnComplexes(static_cast<Complex<T> &>(result).stdComplex(), static_cast<Complex<T> &>(nextOperandEvaluation).stdComplex(), approximationContext.complexFormat());
      } else if (nextOperandEvaluation.type() == EvaluationNode<T>::Type::ListComplex) {
        if (!mapOnList) {
          return Complex<T>::Undefined();
        }
        intermediateResult = DistributeComplexOverList<T>(static_cast<Complex<T> &>(result).stdComplex(), static_cast<ListComplex<T> &>(nextOperandEvaluation), approximationContext.complexFormat(), computeOnComplexes, true);
      } else {
        assert(nextOperandEvaluation.type() == EvaluationNode<T>::Type::MatrixComplex);
        intermediateResult = computeOnComplexAndMatrix(static_cast<Complex<T> &>(result).stdComplex(), static_cast<MatrixComplex<T> &>(nextOperandEvaluation), approximationContext.complexFormat());
      }
    // If element is list
    } else if (result.type() == EvaluationNode<T>::Type::ListComplex) {
      if (!mapOnList) {
        return Complex<T>::Undefined();
      }
      if (nextOperandEvaluation.type() == EvaluationNode<T>::Type::Complex) {
        intermediateResult = DistributeComplexOverList<T>(static_cast<Complex<T> &>(nextOperandEvaluation).stdComplex(), static_cast<ListComplex<T> &>(result), approximationContext.complexFormat(), computeOnComplexes, false);
      } else if (nextOperandEvaluation.type() == EvaluationNode<T>::Type::ListComplex) {
        // Matrices and lists are not compatible
        intermediateResult = DistributeListOverList<T>(static_cast<ListComplex<T> &>(result), static_cast<ListComplex<T> &>(nextOperandEvaluation), approximationContext.complexFormat(), computeOnComplexes);
      } else {
        // Matrices and lists are not compatible
        assert(nextOperandEvaluation.type() == EvaluationNode<T>::Type::MatrixComplex);
        return Complex<T>::Undefined();
      }
    // If element if matrix
    } else {
      assert(result.type() == EvaluationNode<T>::Type::MatrixComplex);
      if (nextOperandEvaluation.type() == EvaluationNode<T>::Type::Complex) {
        intermediateResult = computeOnMatrixAndComplex(static_cast<MatrixComplex<T> &>(result), static_cast<Complex<T> &>(nextOperandEvaluation).stdComplex(), approximationContext.complexFormat());
      } else if (nextOperandEvaluation.type() == EvaluationNode<T>::Type::MatrixComplex) {
        intermediateResult = computeOnMatrices(static_cast<MatrixComplex<T> &>(result), static_cast<MatrixComplex<T> &>(nextOperandEvaluation), approximationContext.complexFormat());
      } else {
        // Matrices and lists are not compatible
        assert(nextOperandEvaluation.type() == EvaluationNode<T>::Type::ListComplex);
        return Complex<T>::Undefined();
      }
    }
    result = intermediateResult;
    if (result.isUndefined()) {
      return Complex<T>::Undefined();
    }
  }
  return result;
}

template<typename T> MatrixComplex<T> ApproximationHelper::ElementWiseOnMatrixComplexAndComplex(const MatrixComplex<T> m, const std::complex<T> c, Poincare::Preferences::ComplexFormat complexFormat, ComplexAndComplexReduction<T> computeOnComplexes) {
  MatrixComplex<T> matrix = MatrixComplex<T>::Builder();
  for (int i = 0; i < m.numberOfChildren(); i++) {
    matrix.addChildAtIndexInPlace(computeOnComplexes(m.complexAtIndex(i), c, complexFormat), i, i);
  }
  matrix.setDimensions(m.numberOfRows(), m.numberOfColumns());
  return matrix;
}

template<typename T> MatrixComplex<T> ApproximationHelper::ElementWiseOnComplexMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, Poincare::Preferences::ComplexFormat complexFormat, ComplexAndComplexReduction<T> computeOnComplexes) {
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

template<typename T> ListComplex<T> ApproximationHelper::DistributeComplexOverList(const std::complex<T> c, const ListComplex<T> l, Preferences::ComplexFormat complexFormat, ComplexAndComplexReduction<T> computeOnComplexes, bool complexFirst) {
  // TODO :
  return ListComplex<T>::Undefined();
}

template<typename T> ListComplex<T> ApproximationHelper::DistributeListOverList(const ListComplex<T> l1, const ListComplex<T> l2, Preferences::ComplexFormat complexFormat, ComplexAndComplexReduction<T> computeOnComplexes) {
  // TODO
  return ListComplex<T>::Undefined();
}

template bool Poincare::ApproximationHelper::IsIntegerRepresentationAccurate<float>(float x);
template bool Poincare::ApproximationHelper::IsIntegerRepresentationAccurate<double>(double x);

template uint32_t Poincare::ApproximationHelper::PositiveIntegerApproximationIfPossible<float>(Poincare::ExpressionNode const*, bool*, ExpressionNode::ApproximationContext);
template uint32_t Poincare::ApproximationHelper::PositiveIntegerApproximationIfPossible<double>(Poincare::ExpressionNode const*, bool*, ExpressionNode::ApproximationContext);

template std::complex<float> Poincare::ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable<float>(std::complex<float>,std::complex<float>,std::complex<float>,bool);
template std::complex<double> Poincare::ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable<double>(std::complex<double>,std::complex<double>,std::complex<double>,bool);

template Poincare::Evaluation<float> Poincare::ApproximationHelper::Map(const Poincare::ExpressionNode * expression, ExpressionNode::ApproximationContext, Poincare::ApproximationHelper::ComplexCompute<float> compute, bool mapOnMatrix, bool mapOnList);
template Poincare::Evaluation<double> Poincare::ApproximationHelper::Map(const Poincare::ExpressionNode * expression, ExpressionNode::ApproximationContext, Poincare::ApproximationHelper::ComplexCompute<double> compute, bool mapOnMatrix, bool mapOnList);

template Poincare::Evaluation<float> Poincare::ApproximationHelper::MapReduce(const Poincare::ExpressionNode * expression, ExpressionNode::ApproximationContext, Poincare::ApproximationHelper::ComplexAndComplexReduction<float> computeOnComplexes, Poincare::ApproximationHelper::ComplexAndMatrixReduction<float> computeOnComplexAndMatrix, Poincare::ApproximationHelper::MatrixAndComplexReduction<float> computeOnMatrixAndComplex, Poincare::ApproximationHelper::MatrixAndMatrixReduction<float> computeOnMatrices, bool mapOnList);
template Poincare::Evaluation<double> Poincare::ApproximationHelper::MapReduce(const Poincare::ExpressionNode * expression, ExpressionNode::ApproximationContext, Poincare::ApproximationHelper::ComplexAndComplexReduction<double> computeOnComplexes, Poincare::ApproximationHelper::ComplexAndMatrixReduction<double> computeOnComplexAndMatrix, Poincare::ApproximationHelper::MatrixAndComplexReduction<double> computeOnMatrixAndComplex, Poincare::ApproximationHelper::MatrixAndMatrixReduction<double> computeOnMatrices, bool mapOnList);

template Poincare::MatrixComplex<float> Poincare::ApproximationHelper::ElementWiseOnMatrixComplexAndComplex<float>(const Poincare::MatrixComplex<float>, const std::complex<float>, Poincare::Preferences::ComplexFormat, Poincare::Complex<float> (*)(std::complex<float>, std::complex<float>, Poincare::Preferences::ComplexFormat));
template Poincare::MatrixComplex<double> Poincare::ApproximationHelper::ElementWiseOnMatrixComplexAndComplex<double>(const Poincare::MatrixComplex<double>, std::complex<double> const, Poincare::Preferences::ComplexFormat, Poincare::Complex<double> (*)(std::complex<double>, std::complex<double>, Poincare::Preferences::ComplexFormat));

template Poincare::MatrixComplex<float> Poincare::ApproximationHelper::ElementWiseOnComplexMatrices<float>(const Poincare::MatrixComplex<float>, const Poincare::MatrixComplex<float>, Poincare::Preferences::ComplexFormat, Poincare::Complex<float> (*)(std::complex<float>, std::complex<float>, Poincare::Preferences::ComplexFormat));
template Poincare::MatrixComplex<double> Poincare::ApproximationHelper::ElementWiseOnComplexMatrices<double>(const Poincare::MatrixComplex<double>, const Poincare::MatrixComplex<double>, Poincare::Preferences::ComplexFormat, Poincare::Complex<double> (*)(std::complex<double>, std::complex<double>, Poincare::Preferences::ComplexFormat));

template Poincare::ListComplex<float> Poincare::ApproximationHelper::DistributeComplexOverList<float>(const std::complex<float> c, const Poincare::ListComplex<float> l, Poincare::Preferences::ComplexFormat complexFormat, Poincare::ApproximationHelper::ComplexAndComplexReduction<float> computeOnComplexes, bool complexFirst);
template Poincare::ListComplex<double> Poincare::ApproximationHelper::DistributeComplexOverList<double>(const std::complex<double> c, const Poincare::ListComplex<double> l, Poincare::Preferences::ComplexFormat complexFormat, Poincare::ApproximationHelper::ComplexAndComplexReduction<double> computeOnComplexes, bool complexFirst);

template Poincare::ListComplex<float> Poincare::ApproximationHelper::DistributeListOverList<float>(const Poincare::ListComplex<float> l1, const Poincare::ListComplex<float> l2, Poincare::Preferences::ComplexFormat complexFormat, Poincare::ApproximationHelper::ComplexAndComplexReduction<float> computeOnComplexes);
template Poincare::ListComplex<double> Poincare::ApproximationHelper::DistributeListOverList<double>(const Poincare::ListComplex<double> l1, const Poincare::ListComplex<double> l2, Poincare::Preferences::ComplexFormat complexFormat, Poincare::ApproximationHelper::ComplexAndComplexReduction<double> computeOnComplexes);

}
