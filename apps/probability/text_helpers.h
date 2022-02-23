#ifndef PROBABILITY_TEXT_HELPERS_H
#define PROBABILITY_TEXT_HELPERS_H

#include <apps/i18n.h>
#include <ion/unicode/code_point.h>
#include <poincare/horizontal_layout.h>
#include <stddef.h>

#include "models/data_enums.h"

namespace Probability {

const char * testToTextSymbol(Data::Test t);
I18n::Message titleFormatForTest(Data::Test test, Data::TestType type);
I18n::Message graphTitleFormatForTest(Data::Test test, Data::TestType type, Data::CategoricalType categoricalType);

int defaultConvertFloatToText(double value, char buffer[], int bufferSize);

Poincare::Layout XOneMinusXTwoLayout();

Poincare::HorizontalLayout codePointSubscriptCodePointLayout(CodePoint base, CodePoint subscript);

Poincare::Layout setSmallFont(Poincare::Layout layout);

}  // namespace Probability

#endif /* PROBABILITY_TEXT_HELPERS_H */
