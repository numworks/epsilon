#ifndef POINCARE_APPROXIMATION_HELPER_H
#define POINCARE_APPROXIMATION_HELPER_H

#include <poincare/complex.h>
#include <poincare/list_complex.h>
#include <poincare/matrix_complex.h>
#include <poincare/expression_node.h>
#include <complex.h>

namespace Poincare {

namespace ApproximationHelper {
  constexpr static int k_maxNumberOfParametersForMap = 4;
  template <typename T> T Epsilon();
  template <typename T> bool IsIntegerRepresentationAccurate(T x);
  template <typename T> uint32_t PositiveIntegerApproximationIfPossible(const ExpressionNode * expression, bool * isUndefined, ExpressionNode::ApproximationContext approximationContext);
  template <typename T> std::complex<T> NeglectRealOrImaginaryPartIfNeglectable(std::complex<T> result, std::complex<T> input1, std::complex<T> input2 = 1.0, bool enableNullResult = true);

  template <typename T> using ComplexesCompute = Complex<T>(*)(const std::complex<T> * c, int numberOfComplexes, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, void * context);
  template<typename T> Evaluation<T> Map(const ExpressionNode * expression, ExpressionNode::ApproximationContext approximationContext, ComplexesCompute<T> compute, bool mapOnList = true, void * context = nullptr);

  template <typename T> using ComplexCompute = Complex<T>(*)(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  template<typename T> Evaluation<T> MapOneChild(const ExpressionNode * expression, ExpressionNode::ApproximationContext approximationContext, ComplexCompute<T> compute, bool mapOnList = true);

  template <typename T> using ComplexAndComplexReduction = Complex<T>(*)(const std::complex<T> c1, const std::complex<T> c2, Preferences::ComplexFormat complexFormat);
  template <typename T> using ComplexAndMatrixReduction = MatrixComplex<T>(*)(const std::complex<T> c, const MatrixComplex<T> m, Preferences::ComplexFormat complexFormat);
  template <typename T> using MatrixAndComplexReduction = MatrixComplex<T>(*)(const MatrixComplex<T> m, const std::complex<T> c, Preferences::ComplexFormat complexFormat);
  template <typename T> using MatrixAndMatrixReduction = MatrixComplex<T>(*)(const MatrixComplex<T> m, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat);

  // Undef reduction functions
  template <typename T> MatrixComplex<T> UndefinedOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> m, Preferences::ComplexFormat complexFormat) {
    return MatrixComplex<T>::Undefined();
  }
  template <typename T> MatrixComplex<T> UndefinedOnMatrixAndComplex(const MatrixComplex<T> m, const std::complex<T> c, Preferences::ComplexFormat complexFormat) {
    return MatrixComplex<T>::Undefined();
  }
  template <typename T> MatrixComplex<T> UndefinedOnMatrixAndMatrix(const MatrixComplex<T> m, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat) {
    return MatrixComplex<T>::Undefined();
  }

  template<typename T> Evaluation<T> Reduce(
      Evaluation<T> eval1,
      Evaluation<T> eval2,
      Preferences::ComplexFormat complexFormat,
      ComplexAndComplexReduction<T> computeOnComplexes,
      ComplexAndMatrixReduction<T> computeOnComplexAndMatrix,
      MatrixAndComplexReduction<T> computeOnMatrixAndComplex,
      MatrixAndMatrixReduction<T> computeOnMatrices,
      bool mapOnList = true
      );

  template <typename T> using ReductionFunction = Evaluation<T>(*)(Evaluation<T> eval1, Evaluation<T> eval2, Preferences::ComplexFormat complexFormat);

  template<typename T> Evaluation<T> MapReduce(
      const ExpressionNode * expression,
      ExpressionNode::ApproximationContext approximationContext,
      ReductionFunction<T> reductionFunction
      );

  template<typename T> MatrixComplex<T> ElementWiseOnMatrixComplexAndComplex(const MatrixComplex<T> n, std::complex<T> c, Preferences::ComplexFormat complexFormat, ComplexAndComplexReduction<T> computeOnComplexes);
  template<typename T> MatrixComplex<T> ElementWiseOnComplexMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat, ComplexAndComplexReduction<T> computeOnComplexes);

  template<typename T> ListComplex<T> DistributeComplexOverList(const std::complex<T> c, const ListComplex<T> l, Preferences::ComplexFormat complexFormat, ComplexAndComplexReduction<T> computeOnComplexes, bool complexFirst);
  template<typename T> ListComplex<T> DistributeListOverList(const ListComplex<T> l1, const ListComplex<T> l2, Preferences::ComplexFormat complexFormat, ComplexAndComplexReduction<T> computeOnComplexes);
};

}

#endif
