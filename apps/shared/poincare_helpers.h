#ifndef SHARED_POINCARE_HELPERS_H
#define SHARED_POINCARE_HELPERS_H

#include <apps/global_preferences.h>
#include <poincare/expression.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/src/expression/projection.h>

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

inline Poincare::ApproximationContext ApproximationContextForParameters(
    const Poincare::Expression e, Poincare::Context* context,
    const ApproximationParameters& approximationParameters) {
  Poincare::ApproximationContext approximationContext(
      context, approximationParameters.complexFormat,
      approximationParameters.angleUnit);
  if (approximationParameters.updateComplexFormatWithExpression) {
    approximationContext.updateComplexFormat(e);
  }
  return approximationContext;
}

// Approximate to tree and keep units
template <class T>
inline Poincare::Expression Approximate(
    const Poincare::Expression e, Poincare::Context* context,
    const ApproximationParameters& approximationParameters = {}) {
  return e.approximateToTree<T>(
      ApproximationContextForParameters(e, context, approximationParameters));
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
  Poincare::UnitConversion unitConversion = Poincare::UnitConversion::Default;
};

inline Poincare::Internal::ProjectionContext ProjectionContextForParameters(
    const Poincare::Expression e, Poincare::Context* context,
    const ReductionParameters& reductionParameters) {
  Poincare::Internal::ProjectionContext projectionContext = {
      .m_complexFormat = reductionParameters.complexFormat,
      .m_angleUnit = reductionParameters.angleUnit,
      .m_expansionStrategy =
          (reductionParameters.target ==
           Poincare::ReductionTarget::SystemForAnalysis)
              ? Poincare::Internal::ExpansionStrategy::ExpandAlgebraic
              : Poincare::Internal::ExpansionStrategy::None,
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

}  // namespace PoincareHelpers

}  // namespace Shared

#endif
