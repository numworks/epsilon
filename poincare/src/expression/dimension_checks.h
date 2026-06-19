#pragma once

#include <poincare/coordinate_2D.h>
#include <poincare/point_or_scalar.h>
#include <poincare/symbol_context.h>

#include <complex>

#include "boolean_or_undefined.h"
#include "k_tree.h"

namespace Poincare::Internal {

struct Dimension;

namespace Approximation {

/* Check that the approximation return type (Tree*, float, Coordinate2D etc.) is
 * compatible with the expression dimension */
template <typename ReturnType>
bool DimensionMatchesReturnType(Dimension dimension);

/* Checks that the expression has a valid dimension, returning true or false. If
 * optimize is true, the checks are asserted instead of returning false in case
 * of failure */
template <typename ReturnType>
bool DimensionChecks(const Tree* e, const SymbolContext& symbolContext,
                     bool isListAllowed, bool optimize);

/* Undefined value representing an unhandled dimension for the T type (Tree,
 * float / double, boolean etc.)
 * TODO: this template method and its specializations can disappear once we find
 * a way to assert the dimension check in To instead of returning an
 * undefined value. */
template <typename T>
T UndefinedUnhandledDimension();

template <>
inline Tree* UndefinedUnhandledDimension() {
  return KUndefUnhandledDimension->cloneTree();
}

template <>
inline std::complex<float> UndefinedUnhandledDimension() {
  return std::complex<float>{NAN};
}

template <>
inline std::complex<double> UndefinedUnhandledDimension() {
  return std::complex<double>{NAN};
}

template <>
inline PointOrRealScalar<float> UndefinedUnhandledDimension() {
  return PointOrRealScalar<float>(NAN, NAN);
}

template <>
inline PointOrRealScalar<double> UndefinedUnhandledDimension() {
  return PointOrRealScalar<double>(NAN, NAN);
}

template <>
inline BooleanOrUndefined UndefinedUnhandledDimension() {
  return BooleanOrUndefined::Undef();
}

template <>
inline float UndefinedUnhandledDimension() {
  return NAN;
}

template <>
inline double UndefinedUnhandledDimension() {
  return NAN;
}

template <>
inline Coordinate2D<float> UndefinedUnhandledDimension() {
  return Coordinate2D<float>{NAN, NAN};
}

template <>
inline Coordinate2D<double> UndefinedUnhandledDimension() {
  return Coordinate2D<double>{NAN, NAN};
}

}  // namespace Approximation

}  // namespace Poincare::Internal
