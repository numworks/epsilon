#ifndef SHARED_POINCARE_HELPERS_H
#define SHARED_POINCARE_HELPERS_H

#include <apps/global_preferences.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/expression.h>

namespace Shared {

namespace PoincareHelpers {

constexpr static Poincare::ExpressionNode::SymbolicComputation k_replaceWithDefinitionOrUndefined = Poincare::ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined;
constexpr static Poincare::ExpressionNode::SymbolicComputation k_replaceWithDefinition = Poincare::ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition;
constexpr static Poincare::ExpressionNode::UnitConversion k_defaultUnitConversion = Poincare::ExpressionNode::UnitConversion::Default;

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

static Poincare::Preferences::AngleUnit AngleUnitForPreferences(Poincare::Preferences * preferences, bool updateAngleUnit, const Poincare::Expression e, Poincare::Context * context) {
  return updateAngleUnit ? Poincare::Preferences::UpdatedAngleUnitWithExpressionInput(preferences->angleUnit(), e, context) : preferences->angleUnit();
}

template <class T>
inline Poincare::Expression Approximate(
  const Poincare::Expression e,
  Poincare::Context * context,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences(),
  bool updateComplexFormatAndAngleUnit = true)
{
  return e.approximate<T>(context, ComplexFormatForPreferences(preferences, updateComplexFormatAndAngleUnit, e, context), AngleUnitForPreferences(preferences, updateComplexFormatAndAngleUnit, e, context));
}

template <class T>
inline T ApproximateToScalar(
  const Poincare::Expression e,
  Poincare::Context * context,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences(),
  bool updateComplexFormatAndAngleUnit = true)
{
  return e.approximateToScalar<T>(context, ComplexFormatForPreferences(preferences, updateComplexFormatAndAngleUnit, e, context), AngleUnitForPreferences(preferences, updateComplexFormatAndAngleUnit, e, context));
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
  return e.approximateWithValueForSymbol<T>(symbol, x, context, ComplexFormatForPreferences(preferences, updateComplexFormatAndAngleUnit, e, context), AngleUnitForPreferences(preferences, updateComplexFormatAndAngleUnit, e, context));
}

// This method automatically update complex format and angle unit
template <class T>
inline T ParseAndSimplifyAndApproximateToScalar(
  const char * text,
  Poincare::Context * context,
  Poincare::ExpressionNode::SymbolicComputation symbolicComputation = k_replaceWithDefinitionOrUndefined,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences())
{
  return Poincare::Expression::ParseAndSimplifyAndApproximateToScalar<T>(text, context, preferences->complexFormat(), preferences->angleUnit(), GlobalPreferences::sharedGlobalPreferences()->unitFormat(), symbolicComputation);
}

// This method automatically update complex format and angle unit
inline Poincare::Expression ParseAndSimplify(
  const char * text,
  Poincare::Context * context,
  Poincare::ExpressionNode::SymbolicComputation symbolicComputation = k_replaceWithDefinitionOrUndefined,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences())
{
  return Poincare::Expression::ParseAndSimplify(text, context, preferences->complexFormat(), preferences->angleUnit(), GlobalPreferences::sharedGlobalPreferences()->unitFormat(), symbolicComputation);
}

inline void CloneAndSimplify(
  Poincare::Expression * e,
  Poincare::Context * context,
  Poincare::ExpressionNode::ReductionTarget target,
  Poincare::ExpressionNode::SymbolicComputation symbolicComputation = k_replaceWithDefinition,
  Poincare::ExpressionNode::UnitConversion unitConversion = k_defaultUnitConversion,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences(),
  bool updateComplexFormatAndAngleUnit = true)
{
  *e = e->cloneAndSimplify(Poincare::ExpressionNode::ReductionContext(context, ComplexFormatForPreferences(preferences, updateComplexFormatAndAngleUnit, *e, context), AngleUnitForPreferences(preferences, updateComplexFormatAndAngleUnit, *e, context), GlobalPreferences::sharedGlobalPreferences()->unitFormat(), target, symbolicComputation, unitConversion));
}

inline void CloneAndReduce(
  Poincare::Expression * e,
  Poincare::Context * context,
  Poincare::ExpressionNode::ReductionTarget target,
  Poincare::ExpressionNode::SymbolicComputation symbolicComputation = k_replaceWithDefinition,
  Poincare::ExpressionNode::UnitConversion unitConversion = k_defaultUnitConversion,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences(),
  bool updateComplexFormatAndAngleUnit = true)
{
  *e = e->cloneAndReduce(Poincare::ExpressionNode::ReductionContext(context, ComplexFormatForPreferences(preferences, updateComplexFormatAndAngleUnit, *e, context), AngleUnitForPreferences(preferences, updateComplexFormatAndAngleUnit, *e, context), GlobalPreferences::sharedGlobalPreferences()->unitFormat(), target, symbolicComputation, unitConversion));
}

inline void ReduceAndRemoveUnit(
  Poincare::Expression * e,
  Poincare::Context * context,
  Poincare::ExpressionNode::ReductionTarget target,
  Poincare::Expression * unit,
  Poincare::ExpressionNode::SymbolicComputation symbolicComputation = k_replaceWithDefinition,
  Poincare::ExpressionNode::UnitConversion unitConversion = k_defaultUnitConversion,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences(),
  bool updateComplexFormatAndAngleUnit = true)
{
  PoincareHelpers::CloneAndReduce(e, context, target, symbolicComputation, unitConversion, preferences, updateComplexFormatAndAngleUnit);
  *e = e->removeUnit(unit);
}

// This method automatically update complex format and angle unit
inline void ParseAndSimplifyAndApproximate(
  const char * text,
  Poincare::Expression * simplifiedExpression,
  Poincare::Expression * approximateExpression,
  Poincare::Context * context,
  Poincare::ExpressionNode::SymbolicComputation symbolicComputation = k_replaceWithDefinitionOrUndefined,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences())
{
  Poincare::Expression::ParseAndSimplifyAndApproximate(text, simplifiedExpression, approximateExpression, context, preferences->complexFormat(), preferences->angleUnit(), GlobalPreferences::sharedGlobalPreferences()->unitFormat(), symbolicComputation);
}

inline typename Poincare::Coordinate2D<double> NextMinimum(
  const Poincare::Expression e,
  const char * symbol,
  double start,
  double max,
  Poincare::Context * context,
  double relativePrecision,
  double minimalStep,
  double maximalStep,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences(),
  bool updateComplexFormatAndAngleUnit = true)
{
  return e.nextMinimum(symbol, start, max, context, ComplexFormatForPreferences(preferences, updateComplexFormatAndAngleUnit, e, context), AngleUnitForPreferences(preferences, updateComplexFormatAndAngleUnit, e, context), relativePrecision, minimalStep, maximalStep);
}

inline typename Poincare::Coordinate2D<double> NextMaximum(
  const Poincare::Expression e,
  const char * symbol,
  double start,
  double max,
  Poincare::Context * context,
  double relativePrecision,
  double minimalStep,
  double maximalStep,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences(),
  bool updateComplexFormatAndAngleUnit = true)
{
  return e.nextMaximum(symbol, start, max, context, ComplexFormatForPreferences(preferences, updateComplexFormatAndAngleUnit, e, context), AngleUnitForPreferences(preferences, updateComplexFormatAndAngleUnit, e, context), relativePrecision, minimalStep, maximalStep);
}

inline double NextRoot(
  const Poincare::Expression e,
  const char * symbol,
  double start,
  double max,
  Poincare::Context * context,
  double relativePrecision,
  double minimalStep,
  double maximalStep,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences(),
  bool updateComplexFormatAndAngleUnit = true)
{
  return e.nextRoot(symbol, start, max, context, ComplexFormatForPreferences(preferences, updateComplexFormatAndAngleUnit, e, context), AngleUnitForPreferences(preferences, updateComplexFormatAndAngleUnit, e, context), relativePrecision, minimalStep, maximalStep);
}

inline typename Poincare::Coordinate2D<double> NextIntersection(
  const Poincare::Expression e,
  const char * symbol,
  double start,
  double max,
  Poincare::Context * context,
  const Poincare::Expression expression,
  double relativePrecision,
  double minimalStep,
  double maximalStep,
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences(),
  bool updateComplexFormatAndAngleUnit = true)
{
  return e.nextIntersection(symbol, start, max, context, ComplexFormatForPreferences(preferences, updateComplexFormatAndAngleUnit, e, context), AngleUnitForPreferences(preferences, updateComplexFormatAndAngleUnit, e, context), expression, relativePrecision, minimalStep, maximalStep);
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
