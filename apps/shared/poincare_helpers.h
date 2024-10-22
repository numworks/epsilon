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
inline int ConvertFloatToText(T d, char* buffer, int bufferSize,
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
inline int ConvertFloatToTextWithDisplayMode(
    T d, char* buffer, int bufferSize, int numberOfSignificantDigits,
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

template <class T>
inline Poincare::Expression Approximate(
    const Poincare::Expression e, Poincare::Context* context,
    const ApproximationParameters& approximationParameters = {}) {
  return e.approximateToTree<T>(
      ApproximationContextForParameters(e, context, approximationParameters));
}

template <class T>
inline T ApproximateToScalar(
    const Poincare::Expression e, Poincare::Context* context,
    const ApproximationParameters& approximationParameters = {}) {
  return e.approximateToScalar<T>(
      ApproximationContextForParameters(e, context, approximationParameters));
}

template <class T>
inline T ApproximateWithValueForSymbol(
    const Poincare::SystemFunction e, const char* symbol, T x,
    Poincare::Context* context,
    const ApproximationParameters& approximationParameters = {}) {
  return e.approximateToScalarWithValueForSymbol<T>(
      symbol, x,
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
      Poincare::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition;
  Poincare::UnitConversion unitConversion = Poincare::UnitConversion::Default;
};

inline Poincare::ReductionContext ReductionContextForParameters(
    const Poincare::Expression e, Poincare::Context* context,
    const ReductionParameters& reductionParameters) {
  Poincare::ReductionContext reductionContext(
      context, reductionParameters.complexFormat, reductionParameters.angleUnit,
      GlobalPreferences::SharedGlobalPreferences()->unitFormat(),
      reductionParameters.target, reductionParameters.symbolicComputation,
      reductionParameters.unitConversion);
  if (reductionParameters.updateComplexFormatWithExpression) {
    reductionContext.updateComplexFormat(e);
  }
  return reductionContext;
}

inline Poincare::Internal::ProjectionContext ProjectionContextForParameters(
    const Poincare::Expression e, Poincare::Context* context,
    const ReductionParameters& reductionParameters) {
  Poincare::ReductionContext reductionContext =
      ReductionContextForParameters(e, context, reductionParameters);
  Poincare::Internal::ProjectionContext projectionContext = {
      .m_complexFormat = reductionContext.complexFormat(),
      .m_angleUnit = reductionContext.angleUnit(),
      .m_unitFormat = reductionContext.unitFormat(),
      .m_symbolic = reductionContext.symbolicComputation(),
      .m_context = reductionContext.context()};
  return projectionContext;
}

template <class T>
inline Poincare::Expression ApproximateKeepingUnits(
    const Poincare::Expression e, Poincare::Context* context,
    const ReductionParameters& reductionParameters = {}) {
#if 0  // TODO_PCJ
  return e.approximateKeepingUnits<T>(
      ReductionContextForParameters(e, context, reductionParameters));
#else
  return Approximate<T>(e, context);

#endif
}

inline void CloneAndSimplify(
    Poincare::Expression* e, Poincare::Context* context,
    const ReductionParameters& reductionParameters = {}) {
  Poincare::Internal::ProjectionContext ctx =
      ProjectionContextForParameters(*e, context, reductionParameters);
  *e = e->cloneAndSimplify(&ctx);
  assert(!e->isUninitialized());
}

inline Poincare::SystemExpression CloneAndReduce(
    Poincare::UserExpression e, Poincare::Context* context,
    const ReductionParameters& reductionParameters = {}) {
  return e.cloneAndReduce(
      ReductionContextForParameters(e, context, reductionParameters));
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
  return Poincare::Expression::ParseAndSimplifyAndApproximateToScalar<T>(
      buffer, context);
}

}  // namespace PoincareHelpers

}  // namespace Shared

#endif
