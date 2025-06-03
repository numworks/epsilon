#ifndef SHARED_POINCARE_HELPERS_H
#define SHARED_POINCARE_HELPERS_H

#include <apps/global_preferences.h>
#include <poincare/expression.h>
#include <poincare/expression_or_float.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/src/expression/projection.h>

#include <type_traits>

namespace Shared {

namespace PoincareHelpers {

// ===== Layout =====

inline Poincare::Layout CreateLayout(
    const Poincare::UserExpression e, Poincare::Context* context,
    Poincare::Preferences::PrintFloatMode displayMode =
        Poincare::Preferences::SharedPreferences()->displayMode(),
    uint8_t numberOfSignificantDigits =
        Poincare::Preferences::SharedPreferences()
            ->numberOfSignificantDigits()) {
  return e.createLayout(displayMode, numberOfSignificantDigits, context);
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
             Poincare::Preferences::SharedPreferences()->displayMode())
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

// ===== Approximation =====

struct ApproximationParameters {
  Poincare::Preferences::ComplexFormat complexFormat =
      Poincare::Preferences::SharedPreferences()->complexFormat();
  Poincare::Preferences::AngleUnit angleUnit =
      Poincare::Preferences::SharedPreferences()->angleUnit();
  bool updateComplexFormatWithExpression = true;
};

// Approximate to tree and keep units
template <class T>
inline Poincare::Expression Approximate(
    Poincare::Expression e, Poincare::Context* context,
    const ApproximationParameters& approximationParameters = {}) {
  Poincare::Preferences::ComplexFormat complexFormat =
      approximationParameters.complexFormat;
  if (approximationParameters.updateComplexFormatWithExpression) {
    complexFormat =
        Poincare::Preferences::UpdatedComplexFormatWithExpressionInput(
            complexFormat, e, context);
  }
  return e.approximateToTree<T>(approximationParameters.angleUnit,
                                complexFormat, context);
}

// Approximate a real scalar expression. Contexts are not handled.
template <class FloatType = float>
inline FloatType ApproximateToRealScalar(Poincare::Expression e) {
  static_assert(std::is_floating_point_v<FloatType>);
  return e.approximateToRealScalar<FloatType>(
      Poincare::Preferences::SharedPreferences()->angleUnit(),
      Poincare::Preferences::SharedPreferences()->complexFormat());
}

// ===== Reduction =====

struct ReductionParameters {
  Poincare::Preferences::ComplexFormat complexFormat =
      Poincare::Preferences::SharedPreferences()->complexFormat();
  Poincare::Preferences::AngleUnit angleUnit =
      Poincare::Preferences::SharedPreferences()->angleUnit();
  bool updateComplexFormatWithExpression = true;

  Poincare::ReductionTarget target = Poincare::ReductionTarget::User;
  Poincare::SymbolicComputation symbolicComputation =
      Poincare::SymbolicComputation::ReplaceDefinedSymbols;
};

inline Poincare::Internal::ProjectionContext ProjectionContextForParameters(
    const Poincare::Expression e, Poincare::Context* context,
    const ReductionParameters& reductionParameters) {
  Poincare::Internal::ProjectionContext projectionContext = {
      .m_complexFormat = reductionParameters.complexFormat,
      .m_angleUnit = reductionParameters.angleUnit,
      .m_reductionTarget = reductionParameters.target,
      .m_unitFormat =
          GlobalPreferences::SharedGlobalPreferences()->unitFormat(),
      .m_symbolic = reductionParameters.symbolicComputation,
      .m_context = context};
  if (reductionParameters.updateComplexFormatWithExpression) {
    Poincare::Internal::Projection::UpdateComplexFormatWithExpressionInput(
        e, &projectionContext);
  }
  return projectionContext;
}

inline void CloneAndSimplify(
    Poincare::Expression* e, Poincare::Context* context,
    const ReductionParameters& reductionParameters = {},
    bool* reductionFailure = nullptr) {
  assert(reductionFailure);

  *e = e->cloneAndSimplify(
      ProjectionContextForParameters(*e, context, reductionParameters),
      reductionFailure);
  assert(!e->isUninitialized());
}

inline Poincare::SystemExpression CloneAndReduce(
    Poincare::UserExpression e, Poincare::Context* context,
    const ReductionParameters& reductionParameters = {},
    bool* reductionFailure = nullptr) {
  assert(reductionFailure);
  return e.cloneAndReduce(
      ProjectionContextForParameters(e, context, reductionParameters),
      reductionFailure);
}

// ===== Misc =====

// Return the nearest number from t's representation with given precision.
template <class T>
inline T ValueOfFloatAsDisplayed(T t, int precision,
                                 Poincare::Context* context) {
  assert(precision <= Poincare::PrintFloat::k_maxNumberOfSignificantDigits);
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
  return Poincare::Expression::ParseAndSimplifyAndApproximateToRealScalar<T>(
      buffer, context,
      Poincare::Preferences::SharedPreferences()->complexFormat(),
      Poincare::Preferences::SharedPreferences()->angleUnit());
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
          .angleUnit = Poincare::Preferences::SharedPreferences()->angleUnit(),
          .complexFormat =
              Poincare::Preferences::SharedPreferences()->complexFormat()});
}

}  // namespace PoincareHelpers

}  // namespace Shared

#endif
