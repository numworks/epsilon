#pragma once

#include <apps/global_preferences.h>
#include <poincare/expression_or_float.h>
#include <poincare/layout.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/src/expression/projection.h>
#include <poincare/symbol_context.h>
#include <poincare/system_expression.h>
#include <poincare/user_expression.h>

#include <type_traits>

#include "global_store.h"

namespace Shared {

namespace PoincareHelpers {

/* These methods are here to interface the Poincare public API with the global
 * parameters fetched from the Epsilon app (user preferences, global context).
 */

// ===== Layout =====

inline Poincare::Layout CreateLayout(
    const Poincare::UserExpression e,
    const Poincare::SymbolContext& symbolContext,
    Poincare::Preferences::PrintFloatMode displayMode =
        GlobalPreferences::SharedGlobalPreferences()->displayMode(),
    uint8_t numberOfSignificantDigits =
        GlobalPreferences::SharedGlobalPreferences()
            ->numberOfSignificantDigits()) {
  return e.createLayout(displayMode, numberOfSignificantDigits, symbolContext);
}

// ===== Serialization =====

template <class T>
inline size_t ConvertFloatToText(T d, char* buffer, size_t bufferSize,
                                 int numberOfSignificantDigits) {
  return Poincare::PrintFloat::ConvertFloatToText(
             d, buffer, bufferSize,
             Poincare::PrintFloat::glyphLengthForFloatWithPrecision(
                 numberOfSignificantDigits),
             numberOfSignificantDigits,
             GlobalPreferences::SharedGlobalPreferences()->displayMode())
      .CharLength;
}

template <class T>
inline size_t ConvertFloatToTextWithDisplayMode(
    T d, char* buffer, size_t bufferSize, int numberOfSignificantDigits,
    Poincare::Preferences::PrintFloatMode displayMode) {
  return Poincare::PrintFloat::ConvertFloatToText(
             d, buffer, bufferSize,
             Poincare::PrintFloat::glyphLengthForFloatWithPrecision(
                 numberOfSignificantDigits),
             numberOfSignificantDigits, displayMode)
      .CharLength;
}

// ===== Simplification and Approximation =====

inline Poincare::ProjectionContext ProjectionContextForPreferences(
    const Poincare::UserExpression e) {
  return Poincare::ProjectionContext{
      .complexFormat = e.preferredComplexFormat(
          GlobalPreferences::SharedGlobalPreferences()->complexFormat(),
          GlobalContextAccessor::Context(),
          Poincare::SymbolicComputation::ReplaceDefinedSymbols),
      .angleUnit = GlobalPreferences::SharedGlobalPreferences()->angleUnit(),
      .m_unitFormat =
          GlobalPreferences::SharedGlobalPreferences()->unitFormat(),
      .m_context = GlobalContextAccessor::Context()};
}

// Approximate a real scalar expression. Contexts are not handled.
template <class FloatType = float>
inline FloatType ApproximateToRealScalar(Poincare::UserExpression e) {
  static_assert(std::is_floating_point_v<FloatType>);
  return e.approximateToRealScalar<FloatType>(
      GlobalPreferences::SharedGlobalPreferences()->angleUnit(),
      GlobalPreferences::SharedGlobalPreferences()->complexFormat());
}

// ===== Misc =====

// Return the nearest number from t's representation with given precision.
template <class T>
inline T ValueOfFloatAsDisplayed(T t, int precision,
                                 const Poincare::SymbolContext& symbolContext =
                                     Poincare::EmptySymbolContext{}) {
  assert(
      precision <=
      static_cast<int>(Poincare::PrintFloat::k_maxNumberOfSignificantDigits));
  constexpr static size_t bufferSize =
      Poincare::PrintFloat::charSizeForFloatsWithPrecision(
          Poincare::PrintFloat::k_maxNumberOfSignificantDigits);
  char buffer[bufferSize];
  // Get displayed value
  size_t numberOfChar = ConvertFloatToText<T>(t, buffer, bufferSize, precision);
  assert(numberOfChar <= bufferSize);
  // Silence compiler warnings for assert
  (void)numberOfChar;
  // Extract displayed value
  return Poincare::UserExpression::ParseAndSimplifyAndApproximateToRealScalar<
      T>(buffer, symbolContext,
         GlobalPreferences::SharedGlobalPreferences()->complexFormat(),
         GlobalPreferences::SharedGlobalPreferences()->angleUnit(),
         Poincare::SymbolicComputation::ReplaceAllSymbolsWithUndefined);
}

// Conversions to float

/* Some controllers are templated to handle a parameter which has either a
 * numeric float type, or Poincare::ExpressionOrFloat (which is a union
 * containing either a Poincare::Expression or a float). A conversion function
 * to a float is needed to take either a direct float type or a
 * Poincare::ExpressionOrFloat as an input. See the ParameterType and FloatType
 * type definitions in the SingleRangeController class for a concrete example.
 */

// Float conversion for direct float types

inline float ToFloat(float value) { return value; }
inline double ToFloat(double value) { return value; }

// Float conversion for Poincare::ExpressionOrFloat

/* When an ExpressionOrFloat contains the expression variant, converting this
 * expression to a float is an approximation. The Poincare approximation API
 * requires an angle unit and a complex format in order to approximate the
 * expression. In the Epsilon apps context, the approximation should use the
 * user settings for angle unit and complex format. */

template <typename FloatType = float>
FloatType ToFloat(Poincare::ExpressionOrFloat value) {
  static_assert(std::is_floating_point_v<FloatType>);
  return value.approximation<FloatType>(
      Poincare::ExpressionOrFloat::ApproximationParameters{
          .angleUnit =
              GlobalPreferences::SharedGlobalPreferences()->angleUnit(),
          .complexFormat =
              GlobalPreferences::SharedGlobalPreferences()->complexFormat()});
}

}  // namespace PoincareHelpers

}  // namespace Shared
