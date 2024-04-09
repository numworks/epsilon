#ifndef SHARED_POINCARE_HELPERS_H
#define SHARED_POINCARE_HELPERS_H

#include <apps/global_preferences.h>
#include <poincare/old/expression.h>
#include <poincare/old/preferences.h>
#include <poincare/old/solver.h>
#include <poincare/print_float.h>

namespace Shared {

namespace PoincareHelpers {

// ===== Layout =====

inline Poincare::Layout CreateLayout(
    const Poincare::Expression e, Poincare::Context* context,
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

inline size_t Serialize(
    const Poincare::Expression e, char* buffer, size_t bufferSize,
    int numberOfSignificantDigits =
        Poincare::PrintFloat::k_maxNumberOfSignificantDigits) {
  return e.serialize(buffer, bufferSize,
                     Poincare::Preferences::SharedPreferences()->displayMode(),
                     numberOfSignificantDigits);
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
  return e.approximate<T>(
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
    const Poincare::Expression e, const char* symbol, T x,
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

template <class T>
inline Poincare::Expression ApproximateKeepingUnits(
    const Poincare::Expression e, Poincare::Context* context,
    const ReductionParameters& reductionParameters = {}) {
  return e.approximateKeepingUnits<T>(
      ReductionContextForParameters(e, context, reductionParameters));
}

inline void CloneAndSimplify(
    Poincare::Expression* e, Poincare::Context* context,
    const ReductionParameters& reductionParameters = {},
    bool* reductionFailure = nullptr) {
  *e = e->cloneAndSimplify(
      ReductionContextForParameters(*e, context, reductionParameters),
      reductionFailure);
}

inline void CloneAndSimplifyAndApproximate(
    Poincare::Expression e, Poincare::Expression* simplifiedExpression,
    Poincare::Expression* approximatedExpression, Poincare::Context* context,
    const ReductionParameters& reductionParameters = {}) {
  e.cloneAndSimplifyAndApproximate(
      simplifiedExpression, approximatedExpression,
      ReductionContextForParameters(e, context, reductionParameters));
}

inline void CloneAndReduce(
    Poincare::Expression* e, Poincare::Context* context,
    const ReductionParameters& reductionParameters = {}) {
  *e = e->cloneAndReduce(
      ReductionContextForParameters(*e, context, reductionParameters));
}

inline void CloneAndApproximateKeepingSymbols(
    Poincare::Expression* e, Poincare::Context* context,
    const ReductionParameters& reductionParameters = {}) {
  *e = e->cloneAndApproximateKeepingSymbols(
      ReductionContextForParameters(*e, context, reductionParameters));
}

inline void CloneAndReduceAndRemoveUnit(
    Poincare::Expression* e, Poincare::Expression* unit,
    Poincare::Context* context,
    const ReductionParameters& reductionParameters = {}) {
  *e = e->cloneAndReduceAndRemoveUnit(
      ReductionContextForParameters(*e, context, reductionParameters), unit);
}

// ===== Misc =====

template <typename T>
inline Poincare::Solver<T> Solver(T xMin, T xMax, const char* unknown = nullptr,
                                  Poincare::Context* context = nullptr) {
  Poincare::Preferences* preferences =
      Poincare::Preferences::SharedPreferences();
  return Poincare::Solver<T>(xMin, xMax, unknown, context,
                             preferences->complexFormat(),
                             preferences->angleUnit());
}

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
