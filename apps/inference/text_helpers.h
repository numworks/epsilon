#ifndef INFERENCE_TEXT_HELPERS_H
#define INFERENCE_TEXT_HELPERS_H

#include <apps/i18n.h>
#include <ion/unicode/code_point.h>
#include <poincare/horizontal_layout.h>
#include "inference/constants.h"
#include "shared/poincare_helpers.h"
#include <stddef.h>

namespace Inference {

inline int defaultConvertFloatToText(double value, char buffer[], int bufferSize) { return Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode(value, buffer, bufferSize, Poincare::Preferences::ShortNumberOfSignificantDigits, Poincare::Preferences::PrintFloatMode::Decimal); }

template <typename TextHolder>
void PrintValueInTextHolder(double value, TextHolder * textField, bool forbidNAN = true, bool forbidNegative = false) {
  if ((forbidNAN && std::isnan(value)) || (forbidNegative && value < 0.0)) {
    textField->setText("");
  } else {
    constexpr int bufferSize = Constants::k_shortBufferSize;
    char buffer[bufferSize];
    defaultConvertFloatToText(value, buffer, bufferSize);
    textField->setText(buffer);
  }
}

Poincare::Layout XOneMinusXTwoLayout();

}

#endif
