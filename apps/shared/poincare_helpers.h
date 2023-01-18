#ifndef SHARED_POINCARE_HELPERS_H
#define SHARED_POINCARE_HELPERS_H

#include <apps/global_preferences.h>
#include <poincare/expression.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/solver.h>

namespace Shared {

namespace PoincareHelpers {

constexpr static Poincare::SymbolicComputation k_replaceWithDefinitionOrUndefined = Poincare::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined;
constexpr static Poincare::SymbolicComputation k_replaceWithDefinition = Poincare::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition;
constexpr static Poincare::UnitConversion k_defaultUnitConversion = Poincare::UnitConversion::Default;

inline Poincare::Layout CreateLayout(
  const Poincare::Expression e,
  Poincare::Context * context,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences())
{
  return e.createLayout(preferences->displayMode(), preferences->numberOfSignificantDigits(), context);
}

template <class T>
inline int ConvertFloatToText(
  T d,
  char * buffer,
  int bufferSize,
  int numberOfSignificantDigits)
{
  return Poincare::PrintFloat::ConvertFloatToText(d, buffer, bufferSize, Poincare::PrintFloat::glyphLengthForFloatWithPrecision(numberOfSignificantDigits), numberOfSignificantDigits, Poincare::Preferences::sharedPreferences()->displayMode()).CharLength;
}

template <class T>
inline int ConvertFloatToTextWithDisplayMode(
  T d,
  char * buffer,
  int bufferSize,
  int numberOfSignificantDigits,
  Poincare::Preferences::PrintFloatMode displayMode)
{
  return Poincare::PrintFloat::ConvertFloatToText(d, buffer, bufferSize, Poincare::PrintFloat::glyphLengthForFloatWithPrecision(numberOfSignificantDigits), numberOfSignificantDigits, displayMode).CharLength;
}

inline int Serialize(
  const Poincare::Expression e,
  char * buffer, int bufferSize,
  int numberOfSignificantDigits = Poincare::PrintFloat::k_numberOfStoredSignificantDigits)
{
  return e.serialize(buffer, bufferSize, Poincare::Preferences::sharedPreferences()->displayMode(), numberOfSignificantDigits);
}

static Poincare::Preferences::ComplexFormat ComplexFormatForPreferences(Poincare::Preferences * preferences, bool updateComplexFormat, const Poincare::Expression e, Poincare::Context * context) {
  return updateComplexFormat ? Poincare::Preferences::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), e, context) : preferences->complexFormat();
}

template <class T>
inline Poincare::Expression Approximate(
  const Poincare::Expression e,
  Poincare::Context * context,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences(),
  bool updateComplexFormat = true)
{
  return e.approximate<T>(context, ComplexFormatForPreferences(preferences, updateComplexFormat, e, context), preferences->angleUnit());
}

template <class T>
inline Poincare::Expression ApproximateKeepingUnits(
  const Poincare::Expression e,
  Poincare::Context * context,
  Poincare::ReductionTarget target = Poincare::ReductionTarget::User,
  Poincare::SymbolicComputation symbolicComputation = k_replaceWithDefinition,
  Poincare::UnitConversion unitConversion = k_defaultUnitConversion,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences(),
  bool updateComplexFormat = true)
{
  return e.approximateKeepingUnits<T>(Poincare::ReductionContext(context, ComplexFormatForPreferences(preferences, updateComplexFormat, e, context), preferences->angleUnit(), GlobalPreferences::sharedGlobalPreferences()->unitFormat(), target, symbolicComputation, unitConversion));
}

template <class T>
inline T ApproximateToScalar(
  const Poincare::Expression e,
  Poincare::Context * context,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences(),
  bool updateComplexFormatAndAngleUnit = true)
{
  return e.approximateToScalar<T>(context, ComplexFormatForPreferences(preferences, updateComplexFormatAndAngleUnit, e, context), preferences->angleUnit());
}

template <class T>
inline T ApproximateWithValueForSymbol(
  const Poincare::Expression e,
  const char * symbol,
  T x,
  Poincare::Context * context,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences(),
  bool updateComplexFormatAndAngleUnit = true)
{
  return e.approximateWithValueForSymbol<T>(symbol, x, context, ComplexFormatForPreferences(preferences, updateComplexFormatAndAngleUnit, e, context), preferences->angleUnit());
}

// This method automatically updates complex format and angle unit
template <class T>
inline T ParseAndSimplifyAndApproximateToScalar(
  const char * text,
  Poincare::Context * context,
  Poincare::SymbolicComputation symbolicComputation = k_replaceWithDefinitionOrUndefined,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences())
{
  return Poincare::Expression::ParseAndSimplifyAndApproximateToScalar<T>(text, context, preferences->complexFormat(), preferences->angleUnit(), GlobalPreferences::sharedGlobalPreferences()->unitFormat(), symbolicComputation);
}

// This method automatically updates complex format and angle unit
inline Poincare::Expression ParseAndSimplify(
  const char * text,
  Poincare::Context * context,
  Poincare::SymbolicComputation symbolicComputation = k_replaceWithDefinitionOrUndefined,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences())
{
  return Poincare::Expression::ParseAndSimplify(text, context, preferences->complexFormat(), preferences->angleUnit(), GlobalPreferences::sharedGlobalPreferences()->unitFormat(), symbolicComputation);
}

inline void CloneAndSimplify(
  Poincare::Expression * e,
  Poincare::Context * context,
  Poincare::ReductionTarget target,
  Poincare::SymbolicComputation symbolicComputation = k_replaceWithDefinition,
  Poincare::UnitConversion unitConversion = k_defaultUnitConversion,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences(),
  bool updateComplexFormatAndAngleUnit = true)
{
  *e = e->cloneAndSimplify(Poincare::ReductionContext(context, ComplexFormatForPreferences(preferences, updateComplexFormatAndAngleUnit, *e, context), preferences->angleUnit(), GlobalPreferences::sharedGlobalPreferences()->unitFormat(), target, symbolicComputation, unitConversion));
}

inline void CloneAndSimplifyAndApproximate(
  Poincare::Expression e,
  Poincare::Expression * simplifiedExpression,
  Poincare::Expression * approximatedExpression,
  Poincare::Context * context,
  Poincare::SymbolicComputation symbolicComputation = k_replaceWithDefinition,
  Poincare::UnitConversion unitConversion = k_defaultUnitConversion,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences(),
  bool updateComplexFormatAndAngleUnit = true)
{
  e.cloneAndSimplifyAndApproximate(simplifiedExpression, approximatedExpression, context, ComplexFormatForPreferences(preferences, updateComplexFormatAndAngleUnit, e, context), preferences->angleUnit(), GlobalPreferences::sharedGlobalPreferences()->unitFormat(), symbolicComputation, unitConversion);
}

inline void CloneAndReduce(
  Poincare::Expression * e,
  Poincare::Context * context,
  Poincare::ReductionTarget target,
  Poincare::SymbolicComputation symbolicComputation = k_replaceWithDefinition,
  Poincare::UnitConversion unitConversion = k_defaultUnitConversion,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences(),
  bool updateComplexFormatAndAngleUnit = true)
{
  *e = e->cloneAndReduce(Poincare::ReductionContext(context, ComplexFormatForPreferences(preferences, updateComplexFormatAndAngleUnit, *e, context), preferences->angleUnit(), GlobalPreferences::sharedGlobalPreferences()->unitFormat(), target, symbolicComputation, unitConversion));
}

inline void ReduceAndRemoveUnit(
  Poincare::Expression * e,
  Poincare::Context * context,
  Poincare::ReductionTarget target,
  Poincare::Expression * unit,
  Poincare::SymbolicComputation symbolicComputation = k_replaceWithDefinition,
  Poincare::UnitConversion unitConversion = k_defaultUnitConversion,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences(),
  bool updateComplexFormatAndAngleUnit = true)
{
  PoincareHelpers::CloneAndReduce(e, context, target, symbolicComputation, unitConversion, preferences, updateComplexFormatAndAngleUnit);
  *e = e->removeUnit(unit);
}

// This method automatically updates complex format and angle unit
inline void ParseAndSimplifyAndApproximate(
  const char * text,
  Poincare::Expression * parsedExpression,
  Poincare::Expression * simplifiedExpression,
  Poincare::Expression * approximateExpression,
  Poincare::Context * context,
  Poincare::SymbolicComputation symbolicComputation = k_replaceWithDefinitionOrUndefined,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences())
{
  Poincare::Expression::ParseAndSimplifyAndApproximate(text, parsedExpression, simplifiedExpression, approximateExpression, context, preferences->complexFormat(), preferences->angleUnit(), GlobalPreferences::sharedGlobalPreferences()->unitFormat(), symbolicComputation);
}

template<typename T>
inline Poincare::Solver<T> Solver(T xMin, T xMax, const char * unknown = nullptr, Poincare::Context * context = nullptr) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  return Poincare::Solver<T>(xMin, xMax, unknown, context, preferences->complexFormat(), preferences->angleUnit());
}

// Return the nearest number from t's representation with given precision.
template <class T>
inline T ValueOfFloatAsDisplayed(T t, int precision, Poincare::Context * context) {
  assert(precision <= Poincare::PrintFloat::k_numberOfStoredSignificantDigits);
  constexpr static size_t bufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(Poincare::PrintFloat::k_numberOfStoredSignificantDigits);
  char buffer[bufferSize];
  // Get displayed value
  size_t numberOfChar = ConvertFloatToText<T>(t, buffer, bufferSize, precision);
  assert(numberOfChar <= bufferSize);
  // Silence compiler warnings for assert
  (void) numberOfChar;
  // Extract displayed value
  return ParseAndSimplifyAndApproximateToScalar<T>(buffer, context);
}

}

}

#endif
