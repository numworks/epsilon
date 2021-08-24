#ifndef APPS_PROBABILITY_TEXT_HELPERS_H
#define APPS_PROBABILITY_TEXT_HELPERS_H

#include <stddef.h>

#include "models/data_enums.h"

namespace Probability {

/* Fake snprintf, only supports %s or %c */
int snprintf(char * buffer, size_t bufferSize, const char * format, ...);

const char * testToText(Data::Test t);
const char * testToTextSymbol(Data::Test t);
const char * testTypeToText(Data::TestType t);

template <typename T>
int defaultParseFloat(T value, char buffer[], int bufferSize);

int constexpr stringLength(const char * str);

int constexpr stringLength(const char * str) {
  return *str ? 1 + stringLength(str + 1) : 0;
}

}  // namespace Probability

#endif /* APPS_PROBABILITY_TEXT_HELPERS_H */
