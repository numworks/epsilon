#ifndef POINCARE_APPROXIMATION_HELPER_H
#define POINCARE_APPROXIMATION_HELPER_H

#include <poincare/complex.h>
#include <poincare/boolean.h>
#include <poincare/list_complex.h>
#include <poincare/matrix_complex.h>
#include <poincare/expression_node.h>
#include <complex.h>

namespace Poincare {

namespace ApproximationHelper {
  template <typename T> T Epsilon();
  template <typename T> bool IsIntegerRepresentationAccurate(T x);
  template <typename T> uint32_t PositiveIntegerApproximationIfPossible(const ExpressionNode * expression, bool * isUndefined, const ApproximationContext& approximationContext);
  template <typename T> std::complex<T> NeglectRealOrImaginaryPartIfNeglectable(std::complex<T> result, std::complex<T> input1, std::complex<T> input2 = 1.0, bool enableNullResult = true);
  template <typename T> std::complex<T> MakeResultRealIfInputIsReal(std::complex<T> result, std::complex<T> input);

  // Map on mutliple children
  template <typename T> using ComplexesCompute = Complex<T>(*)(const std::complex<T> * c, int numberOfComplexes, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, void * context);
  template <typename T> using BooleansCompute = Evaluation<T>(*)(const bool * b, int numberOfBooleans, void * context);
  template <typename T> Evaluation<T> UndefinedOnBooleans(const bool * b, int numberOfBooleans, void * context) { return Complex<T>::Undefined(); }
  template<typename T> Evaluation<T> Map(const ExpressionNode * expression, const ApproximationContext& approximationContext, ComplexesCompute<T> compute, BooleansCompute<T> booleansCompute = UndefinedOnBooleans, bool mapOnList = true, void * context = nullptr);

  // Map on one child
  template <typename T> using ComplexCompute = Complex<T>(*)(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  template <typename T> using BooleanCompute = Evaluation<T>(*)(const bool b);
  template <typename T> Evaluation<T> UndefinedOnBoolean(const bool b) { return Complex<T>::Undefined(); }
  template<typename T> Evaluation<T> MapOneChild(const ExpressionNode * expression, const ApproximationContext& approximationContext, ComplexCompute<T> compute, BooleanCompute<T> booleanCompute = UndefinedOnBoolean, bool mapOnList = true);

  // Lambda computation function
  template <typename T> using ComplexAndComplexReduction = Complex<T>(*)(const std::complex<T> c1, const std::complex<T> c2, Preferences::ComplexFormat complexFormat);
  template <typename T> using ComplexAndMatrixReduction = MatrixComplex<T>(*)(const std::complex<T> c, const MatrixComplex<T> m, Preferences::ComplexFormat complexFormat);
  template <typename T> using MatrixAndComplexReduction = MatrixComplex<T>(*)(const MatrixComplex<T> m, const std::complex<T> c, Preferences::ComplexFormat complexFormat);
  template <typename T> using MatrixAndMatrixReduction = MatrixComplex<T>(*)(const MatrixComplex<T> m, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat);

  // Undef computation functions
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

  // Lambda reduction function (by default you should use Reduce).
  template <typename T> using ReductionFunction = Evaluation<T>(*)(Evaluation<T> eval1, Evaluation<T> eval2, Preferences::ComplexFormat complexFormat);

  // TODO: For now, MapReduce is always undef on booleans
  template<typename T> Evaluation<T> MapReduce(
      const ExpressionNode * expression,
      const ApproximationContext& approximationContext,
      ReductionFunction<T> reductionFunction
      );

  template<typename T> MatrixComplex<T> ElementWiseOnMatrixAndComplex(const MatrixComplex<T> n, std::complex<T> c, Preferences::ComplexFormat complexFormat, ComplexAndComplexReduction<T> computeOnComplexes);
  template<typename T> MatrixComplex<T> ElementWiseOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat, ComplexAndComplexReduction<T> computeOnComplexes);
};

}

#endif
