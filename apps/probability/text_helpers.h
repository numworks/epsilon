#ifndef APPS_PROBABILITY_TEXT_HELPERS_H
#define APPS_PROBABILITY_TEXT_HELPERS_H

#include <apps/i18n.h>
#include <apps/shared/text_field_delegate_app.h>
#include <assert.h>
#include <escher/container.h>

#include "models/data.h"

namespace Probability {

/* Fake sprintf, only supports %s or %c */
int sprintf(char * buffer, const char * format, ...);

const char * testToText(Data::Test t);
const char * testToTextSymbol(Data::Test t);
const char * testTypeToText(Data::TestType t);


template <typename T>
int defaultParseFloat(T value, char buffer[], int bufferSize);


}  // namespace Probability

#endif /* APPS_PROBABILITY_TEXT_HELPERS_H */
