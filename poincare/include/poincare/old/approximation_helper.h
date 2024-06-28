#ifndef POINCARE_APPROXIMATION_HELPER_H
#define POINCARE_APPROXIMATION_HELPER_H

#include <complex.h>

#include "boolean.h"
#include "complex.h"
#include "expression_node.h"
#include "list_complex.h"
#include "matrix_complex.h"

namespace Poincare {

namespace ApproximationHelper {
template <typename T>
T Epsilon();
template <typename T>
bool IsIntegerRepresentationAccurate(T x);
template <typename T>
uint32_t PositiveIntegerApproximationIfPossible(
    const ExpressionNode* expression, bool* isUndefined,
    const ApproximationContext& approximationContext);

// Map on mutliple children
template <typename T>
using ComplexesCompute =
    std::complex<T> (*)(const std::complex<T>* c, int numberOfComplexes,
                        Preferences::ComplexFormat complexFormat,
                        Preferences::AngleUnit angleUnit, void* context);
template <typename T>
using BooleansCompute = Evaluation<T> (*)(const bool* b, int numberOfBooleans,
                                          void* context);
template <typename T>
Evaluation<T> UndefinedOnBooleans(const bool* b, int numberOfBooleans,
                                  void* context) {
  return Complex<T>::Undefined();
}
template <typename T>
Evaluation<T> Map(const ExpressionNode* expression,
                  const ApproximationContext& approximationContext,
                  ComplexesCompute<T> compute,
                  BooleansCompute<T> booleansCompute = UndefinedOnBooleans,
                  bool mapOnList = true, void* context = nullptr);

// Map on one child
template <typename T>
using ComplexCompute = std::complex<T> (*)(
    const std::complex<T> c, Preferences::ComplexFormat complexFormat,
    Preferences::AngleUnit angleUnit);
template <typename T>
using BooleanCompute = Evaluation<T> (*)(const bool b);
template <typename T>
Evaluation<T> UndefinedOnBoolean(const bool b) {
  return Complex<T>::Undefined();
}

// Lambda computation function
template <typename T>
using ComplexAndComplexReduction =
    std::complex<T> (*)(const std::complex<T> c1, const std::complex<T> c2,
                        Preferences::ComplexFormat complexFormat);
template <typename T>
using ComplexAndMatrixReduction =
    MatrixComplex<T> (*)(const std::complex<T> c, const MatrixComplex<T> m,
                         Preferences::ComplexFormat complexFormat);
template <typename T>
using MatrixAndComplexReduction =
    MatrixComplex<T> (*)(const MatrixComplex<T> m, const std::complex<T> c,
                         Preferences::ComplexFormat complexFormat);
template <typename T>
using MatrixAndMatrixReduction =
    MatrixComplex<T> (*)(const MatrixComplex<T> m, const MatrixComplex<T> n,
                         Preferences::ComplexFormat complexFormat);

// Lambda reduction function (by default you should use Reduce).
template <typename T>
using ReductionFunction =
    Evaluation<T> (*)(Evaluation<T> eval1, Evaluation<T> eval2,
                      Preferences::ComplexFormat complexFormat);

};  // namespace ApproximationHelper

}  // namespace Poincare

#endif
