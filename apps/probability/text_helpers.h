#ifndef PROBABILITY_TEXT_HELPERS_H
#define PROBABILITY_TEXT_HELPERS_H

#include <apps/i18n.h>
#include <ion/unicode/code_point.h>
#include <poincare/horizontal_layout.h>
#include <stddef.h>

#include "models/data_enums.h"

namespace Probability {

/* Fake snprintf, only supports %s or %c */
int snprintf(char * buffer, size_t bufferSize, const char * format, ...);

const char * testToText(Data::Test t);
const char * testToTextSymbol(Data::Test t);
const char * testTypeToText(Data::TestType t);
I18n::Message titleFormatForTest(Data::Test test, Data::TestType type);

void decapitalize(char * text);
void capitalize(char * text);

template <typename T>
int defaultConvertFloatToText(T value, char buffer[], int bufferSize);

Poincare::Layout XOneMinusXTwoLayout();

Poincare::HorizontalLayout codePointSubscriptCodePointLayout(CodePoint base, CodePoint subscript);

Poincare::Layout setSmallFont(Poincare::Layout layout);

}  // namespace Probability

#endif /* PROBABILITY_TEXT_HELPERS_H */
