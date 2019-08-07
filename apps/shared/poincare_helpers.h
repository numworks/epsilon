#ifndef SHARED_POINCARE_HELPERS_H
#define SHARED_POINCARE_HELPERS_H

#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/expression.h>

namespace Shared {

namespace PoincareHelpers {

inline Poincare::Layout CreateLayout(const Poincare::Expression e)  {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  return e.createLayout(preferences->displayMode(), preferences->numberOfSignificantDigits());
}

template <class T>
inline int ConvertFloatToText(T d, char * buffer, int bufferSize, int numberOfSignificantDigits) {
  return Poincare::PrintFloat::ConvertFloatToText(d, buffer, bufferSize, numberOfSignificantDigits, Poincare::Preferences::sharedPreferences()->displayMode());
}

inline int Serialize(const Poincare::Expression e, char * buffer, int bufferSize, int numberOfSignificantDigits = Poincare::PrintFloat::k_numberOfStoredSignificantDigits) {
  return e.serialize(buffer, bufferSize, Poincare::Preferences::sharedPreferences()->displayMode(), numberOfSignificantDigits);
}

template <class T>
inline Poincare::Expression Approximate(const Poincare::Expression e, Poincare::Context * context) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), e, context);
  return e.approximate<T>(context, complexFormat, preferences->angleUnit());
}

template <class T>
inline T ApproximateToScalar(const Poincare::Expression e, Poincare::Context * context) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), e, context);
  return e.approximateToScalar<T>(context, complexFormat, preferences->angleUnit());
}

template <class T>
inline T ApproximateWithValueForSymbol(const Poincare::Expression e, const char * symbol, T x, Poincare::Context * context) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), e, context);
  return e.approximateWithValueForSymbol<T>(symbol, x, context, complexFormat, preferences->angleUnit());
}

template <class T>
inline T ApproximateToScalar(const char * text, Poincare::Context * context, bool symbolicComputation = true) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = Poincare::Expression::UpdatedComplexFormatWithTextInput(preferences->complexFormat(), text);
  return Poincare::Expression::ApproximateToScalar<T>(text, context, complexFormat, preferences->angleUnit(), symbolicComputation);
}

inline Poincare::Expression ParseAndSimplify(const char * text, Poincare::Context * context, bool symbolicComputation = true) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = Poincare::Expression::UpdatedComplexFormatWithTextInput(preferences->complexFormat(), text);
  return Poincare::Expression::ParseAndSimplify(text, context, complexFormat, preferences->angleUnit(), symbolicComputation);
}

inline void Simplify(Poincare::Expression * e, Poincare::Context * context, bool symbolicComputation = true) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), *e, context);
  *e = e->simplify(context, complexFormat, preferences->angleUnit(), symbolicComputation);
}

inline void ParseAndSimplifyAndApproximate(const char * text, Poincare::Expression * simplifiedExpression, Poincare::Expression * approximateExpression, Poincare::Context * context, bool symbolicComputation = true) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = Poincare::Expression::UpdatedComplexFormatWithTextInput(preferences->complexFormat(), text);
  Poincare::Expression::ParseAndSimplifyAndApproximate(text, simplifiedExpression, approximateExpression, context, complexFormat, preferences->angleUnit(), symbolicComputation);
}

inline typename Poincare::Expression::Coordinate2D NextMinimum(const Poincare::Expression e, const char * symbol, double start, double step, double max, Poincare::Context * context) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), e, context);
  return e.nextMinimum(symbol, start, step, max, context, complexFormat, preferences->angleUnit());
}

inline typename Poincare::Expression::Coordinate2D NextMaximum(const Poincare::Expression e, const char * symbol, double start, double step, double max, Poincare::Context * context) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), e, context);
  return e.nextMaximum(symbol, start, step, max, context, complexFormat, preferences->angleUnit());
}

inline double NextRoot(const Poincare::Expression e, const char * symbol, double start, double step, double max, Poincare::Context * context) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), e, context);
  return e.nextRoot(symbol, start, step, max, context, complexFormat, preferences->angleUnit());
}

inline typename Poincare::Expression::Coordinate2D NextIntersection(const Poincare::Expression e, const char * symbol, double start, double step, double max, Poincare::Context * context, const Poincare::Expression expression) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), e, context);
  complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(complexFormat, expression, context);
  return e.nextIntersection(symbol, start, step, max, context, complexFormat, preferences->angleUnit(), expression);
}

}

}

#endif
