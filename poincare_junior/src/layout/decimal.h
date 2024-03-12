#ifndef POINCARE_LAYOUT_DECIMAL_H
#define POINCARE_LAYOUT_DECIMAL_H

#include <poincare/preferences.h>
#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

int ConvertDecimalToText(const Tree* decimal, char* buffer, int bufferSize,
                         Poincare::Preferences::PrintFloatMode mode,
                         int numberOfSignificantDigits);

}  // namespace PoincareJ

#endif
