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
  return Poincare::PrintFloat::convertFloatToText(d, buffer, bufferSize, numberOfSignificantDigits, Poincare::Preferences::sharedPreferences()->displayMode());
}

inline int Serialize(const Poincare::Expression e, char * buffer, int bufferSize, int numberOfSignificantDigits = Poincare::PrintFloat::k_numberOfStoredSignificantDigits) {
  return e.serialize(buffer, bufferSize, Poincare::Preferences::sharedPreferences()->displayMode(), numberOfSignificantDigits);
}

template <class T>
inline Poincare::Expression Approximate(const Poincare::Expression e, Poincare::Context & context) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = preferences->complexFormat();
  complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(complexFormat, e, context);
  return e.approximate<T>(context, complexFormat, preferences->angleUnit());
}

template <class T>
inline T ApproximateToScalar(const Poincare::Expression e, Poincare::Context & context) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = preferences->complexFormat();
  complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(complexFormat, e, context);
  return e.approximateToScalar<T>(context, complexFormat, preferences->angleUnit());
}

template <class T>
inline T ApproximateWithValueForSymbol(const Poincare::Expression e, const char * symbol, T x, Poincare::Context & context) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = preferences->complexFormat();
  complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(complexFormat, e, context);
  return e.approximateWithValueForSymbol<T>(symbol, x, context, complexFormat, preferences->angleUnit());
}

template <class T>
inline T ApproximateToScalar(const char * text, Poincare::Context & context) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat complexFormat = preferences->complexFormat();
  complexFormat = Poincare::Expression::UpdatedComplexFormatWithTextInput(complexFormat, text);
  return Poincare::Expression::approximateToScalar<T>(text, context, complexFormat, preferences->angleUnit());
}

inline Poincare::Expression ParseAndSimplify(const char * text, Poincare::Context & context) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  return Poincare::Expression::ParseAndSimplify(text, context, preferences->complexFormat(), preferences->angleUnit());
}

inline void Simplify(Poincare::Expression * e, Poincare::Context & context, Poincare::Preferences::ComplexFormat complexFormat = Poincare::Preferences::sharedPreferences()->complexFormat()) {
  complexFormat = Poincare::Expression::UpdatedComplexFormatWithExpressionInput(complexFormat, *e, context);
  *e = e->simplify(context, complexFormat, Poincare::Preferences::sharedPreferences()->angleUnit());
}

inline void ParseAndSimplifyAndApproximate(const char * text, Poincare::Expression * simplifiedExpression, Poincare::Expression * approximateExpression, Poincare::Context & context) {
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Expression::ParseAndSimplifyAndApproximate(text, simplifiedExpression, approximateExpression, context, preferences->complexFormat(), preferences->angleUnit());
}

inline void SimplifyAndApproximate(Poincare::Expression * e, Poincare::Expression * approximate,  Poincare::Context & context, Poincare::Preferences::ComplexFormat complexFormat = Poincare::Preferences::sharedPreferences()->complexFormat()) {
  e->simplifyAndApproximate(e, approximate, context, complexFormat, Poincare::Preferences::sharedPreferences()->angleUnit());
}

}

}

#endif

