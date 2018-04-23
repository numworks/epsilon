#ifndef SHARED_POINCARE_HELPERS_H
#define SHARED_POINCARE_HELPERS_H

#include <poincare.h>

namespace Shared {

namespace PoincareHelpers {

inline Poincare::ExpressionLayout * CreateLayout(const Poincare::Expression * e)  {
  return e->createLayout(Poincare::Preferences::sharedPreferences()->displayMode(), Poincare::Preferences::sharedPreferences()->numberOfSignificantDigits());
}

template <class T>
inline int ConvertFloatToText(T d, char * buffer, int bufferSize, int numberOfSignificantDigits) {
  return Poincare::PrintFloat::convertFloatToText(d, buffer, bufferSize, numberOfSignificantDigits, Poincare::Preferences::sharedPreferences()->displayMode());
}

inline int WriteTextInBuffer(const Poincare::Expression * e, char * buffer, int bufferSize, int numberOfSignificantDigits = Poincare::PrintFloat::k_numberOfStoredSignificantDigits) {
  return e->writeTextInBuffer(buffer, bufferSize, Poincare::Preferences::sharedPreferences()->displayMode(), numberOfSignificantDigits);
}

template <class T>
inline Poincare::Expression * Approximate(const Poincare::Expression * e, Poincare::Context & context) {
  return e->approximate<T>(context, Poincare::Preferences::sharedPreferences()->angleUnit(), Poincare::Preferences::sharedPreferences()->complexFormat());
}

template <class T>
inline T ApproximateToScalar(const Poincare::Expression * e, Poincare::Context & context) {
  return e->approximateToScalar<T>(context, Poincare::Preferences::sharedPreferences()->angleUnit());
}

template <class T>
inline T ApproximateToScalar(const char * text, Poincare::Context & context) {
  return Poincare::Expression::approximateToScalar<T>(text, context, Poincare::Preferences::sharedPreferences()->angleUnit());
}

inline Poincare::Expression * ParseAndSimplify(const char * text, Poincare::Context & context) {
  return Poincare::Expression::ParseAndSimplify(text, context, Poincare::Preferences::sharedPreferences()->angleUnit());
}


}

}

#endif

