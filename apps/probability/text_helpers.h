#ifndef PROBABILITY_TEXT_HELPERS_H
#define PROBABILITY_TEXT_HELPERS_H

#include <apps/i18n.h>
#include <ion/unicode/code_point.h>
#include <poincare/horizontal_layout.h>
#include "shared/poincare_helpers.h"
#include <stddef.h>

namespace Probability {

inline int defaultConvertFloatToText(double value, char buffer[], int bufferSize) { return Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode(value, buffer, bufferSize, Poincare::Preferences::ShortNumberOfSignificantDigits, Poincare::Preferences::PrintFloatMode::Decimal); }

Poincare::Layout XOneMinusXTwoLayout();

Poincare::HorizontalLayout codePointSubscriptCodePointLayout(CodePoint base, CodePoint subscript);

}  // namespace Probability

#endif /* PROBABILITY_TEXT_HELPERS_H */
