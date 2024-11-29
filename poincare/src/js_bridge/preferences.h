#ifndef POINCARE_JS_BRIDGE_PREFERENCES_H
#define POINCARE_JS_BRIDGE_PREFERENCES_H

#include <poincare/preferences.h>

namespace Poincare::JSBridge {

/* These preferences are the ones needed to build a reduction context.
 * They are packed in a struct to be passed as a single argument to the
 * builder of ReductionContext, which simplifies their usage in the JS code. */
struct ReductionPreferences {
  Preferences::ComplexFormat complexFormat;
  Preferences::AngleUnit angleUnit;
  Preferences::UnitFormat unitFormat;
};

/* These preferences are the ones needed to turn a UserExpression to latex.
 * They are packed in a struct to be passed as a single argument to the
 * toLatex method of UserExpression, which simplifies their usage in the JS
 * code.
 * */
struct PrintFloatPreferences {
  Preferences::PrintFloatMode printFloatMode;
  int numberOfSignificantDigits;
};

}  // namespace Poincare::JSBridge
#endif
