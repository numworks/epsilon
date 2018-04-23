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

}

}

#endif

