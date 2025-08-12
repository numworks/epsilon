#ifndef SHARED_POINCARE_HELPERS_H
#define SHARED_POINCARE_HELPERS_H

#include <apps/global_preferences.h>
#include <poincare/context.h>
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
        GlobalPreferences::SharedGlobalPreferences()->displayMode(),
    uint8_t numberOfSignificantDigits =
        GlobalPreferences::SharedGlobalPreferences()
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

// ===== Approximation =====

// Approximate to tree and keep units
template <class T>
inline Poincare::SystemExpression ApproximateUser(
    Poincare::UserExpression e, Poincare::Context* context,
    Poincare::Preferences::ComplexFormat complexFormat =
        GlobalPreferences::SharedGlobalPreferences()->complexFormat(),
    Poincare::Preferences::AngleUnit angleUnit =
        GlobalPreferences::SharedGlobalPreferences()->angleUnit()) {
  complexFormat =
      Poincare::Preferences::UpdatedComplexFormatWithExpressionInput(
          complexFormat, e, context);
  return e.approximateUserToTree<T>(
      GlobalPreferences::SharedGlobalPreferences()->angleUnit(), complexFormat,
      context);
}

template <class T>
inline Poincare::SystemExpression ApproximateSystem(
    Poincare::SystemExpression e) {
  return e.approximateSystemToTree<T>();
}

inline Poincare::Internal::ProjectionContext ProjectionContextForPreferences(
    const Poincare::UserExpression e, const Poincare::Context& context) {
  Poincare::Internal::ProjectionContext projectionContext = {
      .m_complexFormat =
          GlobalPreferences::SharedGlobalPreferences()->complexFormat(),
      .m_angleUnit = GlobalPreferences::SharedGlobalPreferences()->angleUnit(),
      .m_unitFormat =
          GlobalPreferences::SharedGlobalPreferences()->unitFormat(),
      .m_context = context};
  Poincare::Internal::Projection::UpdateComplexFormatWithExpressionInput(
      e, &projectionContext);
  return projectionContext;
}

// Approximate a real scalar expression. Contexts are not handled.
template <class FloatType = float>
inline FloatType ApproximateToRealScalar(Poincare::UserExpression e) {
  static_assert(std::is_floating_point_v<FloatType>);
  return e.approximateToRealScalar<FloatType>(
      GlobalPreferences::SharedGlobalPreferences()->angleUnit(),
      GlobalPreferences::SharedGlobalPreferences()->complexFormat());
}

// ===== Reduction =====

inline Poincare::Internal::ProjectionContext ProjectionContextForParameters(
    Poincare::UserExpression e, const Poincare::Context& context,
    Poincare::Preferences::ComplexFormat complexFormat,
    Poincare::Preferences::AngleUnit angleUnit,
    bool updateComplexFormatWithExpression, Poincare::ReductionTarget target,
    Poincare::SymbolicComputation symbolicComputation) {
  Poincare::Internal::ProjectionContext projectionContext = {
      .m_complexFormat = complexFormat,
      .m_angleUnit = angleUnit,
      .m_reductionTarget = target,
      .m_unitFormat =
          GlobalPreferences::SharedGlobalPreferences()->unitFormat(),
      .m_symbolic = symbolicComputation,
      .m_context = context};
  if (updateComplexFormatWithExpression) {
    Poincare::Internal::Projection::UpdateComplexFormatWithExpressionInput(
        e, &projectionContext);
  }
  return projectionContext;
}

inline void CloneAndSimplify(Poincare::UserExpression* e,
                             Poincare::Context* context,
                             Poincare::Preferences::ComplexFormat complexFormat,
                             Poincare::Preferences::AngleUnit angleUnit,
                             bool updateComplexFormatWithExpression,
                             Poincare::ReductionTarget target,
                             Poincare::SymbolicComputation symbolicComputation,
                             bool* reductionFailure) {
  assert(reductionFailure);
  assert(context);
  *e = e->cloneAndSimplify(
      ProjectionContextForParameters(*e, *context, complexFormat, angleUnit,
                                     updateComplexFormatWithExpression, target,
                                     symbolicComputation),
      reductionFailure);
  assert(!e->isUninitialized());
}

inline Poincare::SystemExpression CloneAndReduce(
    Poincare::UserExpression e, Poincare::Context* context,
    Poincare::Preferences::ComplexFormat complexFormat,
    Poincare::Preferences::AngleUnit angleUnit,
    bool updateComplexFormatWithExpression, Poincare::ReductionTarget target,
    Poincare::SymbolicComputation symbolicComputation, bool* reductionFailure) {
  assert(reductionFailure);
  assert(context);
  return e.cloneAndReduce(
      ProjectionContextForParameters(e, *context, complexFormat, angleUnit,
                                     updateComplexFormatWithExpression, target,
                                     symbolicComputation),
      reductionFailure);
}

// ===== Misc =====

// Return the nearest number from t's representation with given precision.
template <class T>
inline T ValueOfFloatAsDisplayed(
    T t, int precision,
    const Poincare::Context& context = Poincare::EmptyContext{}) {
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
      T>(buffer, context,
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

#endif
