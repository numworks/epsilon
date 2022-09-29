#ifndef SHARED_POINCARE_HELPERS_H
#define SHARED_POINCARE_HELPERS_H

#include <apps/global_preferences.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/expression.h>

namespace Shared {

namespace PoincareHelpers {

inline Poincare::Layout CreateLayout(const Poincare::Expression e, Poincare::Context * context)  {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  return e.createLayout(preferences->displayMode(), preferences->numberOfSignificantDigits(), context);
}

template <class T>
inline int ConvertFloatToText(T d, char * buffer, int bufferSize, int numberOfSignificantDigits) {
  return Poincare::PrintFloat::ConvertFloatToText(d, buffer, bufferSize, Poincare::PrintFloat::glyphLengthForFloatWithPrecision(numberOfSignificantDigits), numberOfSignificantDigits, Poincare::Preferences::sharedPreferences()->displayMode()).CharLength;
}

template <class T>
inline int ConvertFloatToTextWithDisplayMode(T d, char * buffer, int bufferSize, int numberOfSignificantDigits, Poincare::Preferences::PrintFloatMode displayMode) {
  return Poincare::PrintFloat::ConvertFloatToText(d, buffer, bufferSize, Poincare::PrintFloat::glyphLengthForFloatWithPrecision(numberOfSignificantDigits), numberOfSignificantDigits, displayMode).CharLength;
}

inline int Serialize(const Poincare::Expression e, char * buffer, int bufferSize, int numberOfSignificantDigits = Poincare::PrintFloat::k_numberOfStoredSignificantDigits) {
  return e.serialize(buffer, bufferSize, Poincare::Preferences::sharedPreferences()->displayMode(), numberOfSignificantDigits);
}

template <class T>
inline Poincare::Expression Approximate(const Poincare::Expression e, Poincare::Context * context) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), e, context);
  Poincare::Preferences::AngleUnit angleUnit = Poincare::Expression::UpdatedAngleUnitWithExpressionInput(preferences->angleUnit(), e, context);
  return e.approximate<T>(context, complexFormat, angleUnit);
}

template <class T>
inline T ApproximateToScalar(const Poincare::Expression e, Poincare::Context * context) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), e, context);
  Poincare::Preferences::AngleUnit angleUnit = Poincare::Expression::UpdatedAngleUnitWithExpressionInput(preferences->angleUnit(), e, context);
  return e.approximateToScalar<T>(context, complexFormat, angleUnit);
}

template <class T>
inline T ApproximateWithValueForSymbol(const Poincare::Expression e, const char * symbol, T x, Poincare::Context * context) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), e, context);
  Poincare::Preferences::AngleUnit angleUnit = Poincare::Expression::UpdatedAngleUnitWithExpressionInput(preferences->angleUnit(), e, context);
  return e.approximateWithValueForSymbol<T>(symbol, x, context, complexFormat, angleUnit);
}

template <class T>
inline T ApproximateToScalar(const char * text, Poincare::Context * context, Poincare::ExpressionNode::SymbolicComputation symbolicComputation = Poincare::ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  return Poincare::Expression::ApproximateToScalar<T>(text, context, preferences->complexFormat(), preferences->angleUnit(), GlobalPreferences::sharedGlobalPreferences()->unitFormat(), symbolicComputation);
}

inline Poincare::Expression ParseAndSimplify(const char * text, Poincare::Context * context, Poincare::ExpressionNode::SymbolicComputation symbolicComputation = Poincare::ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  return Poincare::Expression::ParseAndSimplify(text, context, preferences->complexFormat(), preferences->angleUnit(), GlobalPreferences::sharedGlobalPreferences()->unitFormat(), symbolicComputation);
}

inline void CloneAndSimplify(Poincare::Expression * e, Poincare::Context * context, Poincare::ExpressionNode::ReductionTarget target, Poincare::ExpressionNode::SymbolicComputation symbolicComputation = Poincare::ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, Poincare::ExpressionNode::UnitConversion unitConversion = Poincare::ExpressionNode::UnitConversion::Default) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), *e, context);
  Poincare::Preferences::AngleUnit angleUnit = Poincare::Expression::UpdatedAngleUnitWithExpressionInput(preferences->angleUnit(), *e, context);
  *e = e->cloneAndSimplify(Poincare::ExpressionNode::ReductionContext(context, complexFormat, angleUnit, GlobalPreferences::sharedGlobalPreferences()->unitFormat(), target, symbolicComputation, unitConversion));
}

inline void CloneAndReduce(Poincare::Expression * e, Poincare::Context * context, Poincare::ExpressionNode::ReductionTarget target, Poincare::ExpressionNode::SymbolicComputation symbolicComputation = Poincare::ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, Poincare::ExpressionNode::UnitConversion unitConversion = Poincare::ExpressionNode::UnitConversion::Default) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), *e, context);
  Poincare::Preferences::AngleUnit angleUnit = Poincare::Expression::UpdatedAngleUnitWithExpressionInput(preferences->angleUnit(), *e, context);
  *e = e->cloneAndReduce(Poincare::ExpressionNode::ReductionContext(context, complexFormat, angleUnit, GlobalPreferences::sharedGlobalPreferences()->unitFormat(), target, symbolicComputation, unitConversion));
}

inline void ReduceAndRemoveUnit(Poincare::Expression * e, Poincare::Context * context, Poincare::ExpressionNode::ReductionTarget target, Poincare::Expression * unit, Poincare::ExpressionNode::SymbolicComputation symbolicComputation = Poincare::ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, Poincare::ExpressionNode::UnitConversion unitConversion = Poincare::ExpressionNode::UnitConversion::Default) {
  PoincareHelpers::CloneAndReduce(e, context, target, symbolicComputation, unitConversion);
  *e = e->removeUnit(unit);
}

inline void RemoveUnit(Poincare::Expression * e, Poincare::Context * context, Poincare::ExpressionNode::ReductionTarget target, Poincare::Expression * unit, Poincare::ExpressionNode::SymbolicComputation symbolicComputation = Poincare::ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, Poincare::ExpressionNode::UnitConversion unitConversion = Poincare::ExpressionNode::UnitConversion::Default) {
  *e = e->removeUnit(unit);
}

inline void ParseAndSimplifyAndApproximate(const char * text, Poincare::Expression * simplifiedExpression, Poincare::Expression * approximateExpression, Poincare::Context * context, Poincare::ExpressionNode::SymbolicComputation symbolicComputation = Poincare::ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Expression::ParseAndSimplifyAndApproximate(text, simplifiedExpression, approximateExpression, context, preferences->complexFormat(), preferences->angleUnit(), GlobalPreferences::sharedGlobalPreferences()->unitFormat(), symbolicComputation);
}

inline typename Poincare::Coordinate2D<double> NextMinimum(const Poincare::Expression e, const char * symbol, double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), e, context);
  Poincare::Preferences::AngleUnit angleUnit = Poincare::Expression::UpdatedAngleUnitWithExpressionInput(preferences->angleUnit(), e, context);
  return e.nextMinimum(symbol, start, max, context, complexFormat, angleUnit, relativePrecision, minimalStep, maximalStep);
}

inline typename Poincare::Coordinate2D<double> NextMaximum(const Poincare::Expression e, const char * symbol, double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), e, context);
  Poincare::Preferences::AngleUnit angleUnit = Poincare::Expression::UpdatedAngleUnitWithExpressionInput(preferences->angleUnit(), e, context);
  return e.nextMaximum(symbol, start, max, context, complexFormat, angleUnit, relativePrecision, minimalStep, maximalStep);
}

inline double NextRoot(const Poincare::Expression e, const char * symbol, double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), e, context);
  Poincare::Preferences::AngleUnit angleUnit = Poincare::Expression::UpdatedAngleUnitWithExpressionInput(preferences->angleUnit(), e, context);
  return e.nextRoot(symbol, start, max, context, complexFormat, angleUnit, relativePrecision, minimalStep, maximalStep);
}

inline typename Poincare::Coordinate2D<double> NextIntersection(const Poincare::Expression e, const char * symbol, double start, double max, Poincare::Context * context, const Poincare::Expression expression, double relativePrecision, double minimalStep, double maximalStep) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), e, context);
  Poincare::Preferences::AngleUnit angleUnit = Poincare::Expression::UpdatedAngleUnitWithExpressionInput(preferences->angleUnit(), expression, context);
  return e.nextIntersection(symbol, start, max, context, complexFormat, angleUnit, expression, relativePrecision, minimalStep, maximalStep);
}

}

}

#endif
